/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindTwinBoundaries.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

/**
 * @brief The CalculateTwinBoundaryImpl class implements a threaded algorithm that determines whether a boundary is twin related and calculates
 * the reseptive incoherence.  The calculations are performed on a surface mesh.
 */
class CalculateTwinBoundaryImpl
{
    float m_AxisTol;
    float m_AngTol;
    int32_t* m_Labels;
    double* m_Normals;
    int32_t* m_Phases;
    float* m_Quats;
    bool* m_TwinBoundary;
    float* m_TwinBoundaryIncoherence;
    uint32_t* m_CrystalStructures;
    bool m_FindCoherence;
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;

  public:
    CalculateTwinBoundaryImpl(float angtol, float axistol, int32_t* Labels, double* Normals, float* Quats, int32_t* Phases, unsigned int* CrystalStructures, bool* TwinBoundary, float* TwinBoundaryIncoherence, bool FindCoherence) :
      m_AxisTol(axistol),
      m_AngTol(angtol),
      m_Labels(Labels),
      m_Normals(Normals),
      m_Phases(Phases),
      m_Quats(Quats),
      m_TwinBoundary(TwinBoundary),
      m_TwinBoundaryIncoherence(TwinBoundaryIncoherence),
      m_CrystalStructures(CrystalStructures),
      m_FindCoherence(FindCoherence)
    {
      m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
    }

    virtual ~CalculateTwinBoundaryImpl() {}

    void generate(size_t start, size_t end) const
    {
      int32_t feature1 = 0, feature2 = 0;
      float normal[3] = { 0.0f, 0.0f, 0.0f };
      float g1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
      float w = 0.0f;
      uint32_t phase1 = 0, phase2 = 0;
      QuatF q1 = QuaternionMathF::New();
      QuatF q2 = QuaternionMathF::New();
      float axisdiff111 = 0.0f, angdiff60 = 0.0f;
      float n[3] = { 0.0f, 0.0f, 0.0f };
      float incoherence = 0.0f;
      float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;

      QuatF misq = QuaternionMathF::New();
      QuatF sym_q = QuaternionMathF::New();
      QuatF s1_misq = QuaternionMathF::New();
      QuatF s2_misq = QuaternionMathF::New();
      QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

      float xstl_norm[3] = { 0.0f, 0.0f, 0.0f };
      float s_xstl_norm[3] = { 0.0f, 0.0f, 0.0f };

      for (size_t i = start; i < end; i++)
      {
        feature1 = m_Labels[2 * i];
        feature2 = m_Labels[2 * i + 1];
        if (m_FindCoherence)
        {
          normal[0] = m_Normals[3 * i];
          normal[1] = m_Normals[3 * i + 1];
          normal[2] = m_Normals[3 * i + 2];
        }
        if (feature1 > 0 && feature2 > 0 && m_Phases[feature1] == m_Phases[feature2])
        {
          w = std::numeric_limits<float>::max();

          QuaternionMathF::Copy(quats[feature1], q1);
          QuaternionMathF::Copy(quats[feature2], q2);

          phase1 = m_CrystalStructures[m_Phases[feature1]];
          phase2 = m_CrystalStructures[m_Phases[feature2]];
          if (phase1 == phase2)
          {
            int32_t nsym = m_OrientationOps[phase1]->getNumSymOps();
            QuaternionMathF::Conjugate(q2);
            QuaternionMathF::Multiply(q1, q2, misq);
            FOrientArrayType om(9);
            FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
            om.toGMatrix(g1);

            if (m_FindCoherence) { MatrixMath::Multiply3x3with3x1(g1, normal, xstl_norm); }

            for (int32_t j = 0; j < nsym; j++)
            {
              m_OrientationOps[phase1]->getQuatSymOp(j, sym_q);
              // calculate crystal direction parallel to normal
              QuaternionMathF::Multiply(misq, sym_q, s1_misq);

              if (m_FindCoherence) { QuaternionMathF::MultiplyQuatVec(sym_q, xstl_norm, s_xstl_norm); }

              for (int32_t k = 0; k < nsym; k++)
              {
                // calculate the symmetric misorienation
                m_OrientationOps[phase1]->getQuatSymOp(k, sym_q);
                QuaternionMathF::Conjugate(sym_q);
                QuaternionMathF::Multiply(sym_q, s1_misq, s2_misq);

                FOrientArrayType ax(n1, n2, n3, w);
                FOrientTransformsType::qu2ax(FOrientArrayType(s2_misq), ax);
                ax.toAxisAngle(n1, n2, n3, w);

                w = w * 180.0f / SIMPLib::Constants::k_Pi;
                axisdiff111 = acosf(fabsf(n1) * 0.57735f + fabsf(n2) * 0.57735f + fabsf(n3) * 0.57735f);
                angdiff60 = fabsf(w - 60.0f);
                if (axisdiff111 < m_AxisTol && angdiff60 < m_AngTol)
                {
                  n[0] = n1;
                  n[1] = n2;
                  n[2] = n3;
                  m_TwinBoundary[i] = true;
                  if (m_FindCoherence)
                  {
                    incoherence = 180.0f * acosf(GeometryMath::CosThetaBetweenVectors(n, s_xstl_norm)) / SIMPLib::Constants::k_Pi;
                    if (incoherence > 90.0f) { incoherence = 180.0f - incoherence; }
                    if (incoherence < m_TwinBoundaryIncoherence[i]) { m_TwinBoundaryIncoherence[i] = incoherence; }
                  }
                }
              }
            }
          }
        }
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif
};

// Include the MOC generated file for this class
#include "moc_FindTwinBoundaries.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaries::FindTwinBoundaries() :
  AbstractFilter(),
  m_AxisTolerance(0.0f),
  m_AngleTolerance(0.0f),
  m_FindCoherence(true),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshTwinBoundaryArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundary),
  m_SurfaceMeshTwinBoundaryIncoherenceArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundaryIncoherence),
  m_AvgQuats(NULL),
  m_FeaturePhases(NULL),
  m_CrystalStructures(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormals(NULL),
  m_SurfaceMeshTwinBoundary(NULL),
  m_SurfaceMeshTwinBoundaryIncoherence(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaries::~FindTwinBoundaries()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(DoubleFilterParameter::New("Axis Tolerance (Degrees)", "AxisTolerance", getAxisTolerance(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Angle Tolerance (Degrees)", "AngleTolerance", getAngleTolerance(), FilterParameter::Parameter));
  QStringList linkedProps;
  linkedProps << "SurfaceMeshFaceNormalsArrayPath" << "SurfaceMeshTwinBoundaryIncoherenceArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Compute Coherence", "FindCoherence", getFindCoherence(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Float, 4, DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Average Quaternions", "AvgQuatsArrayPath", getAvgQuatsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::UInt32, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 2, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Double, 3, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Normals", "SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Twin Boundary", "SurfaceMeshTwinBoundaryArrayName", getSurfaceMeshTwinBoundaryArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Twin Boundary Incoherence", "SurfaceMeshTwinBoundaryIncoherenceArrayName", getSurfaceMeshTwinBoundaryIncoherenceArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindTwinBoundaries::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshTwinBoundaryIncoherenceArrayName(reader->readString("SurfaceMeshTwinBoundaryIncoherenceArrayName", getSurfaceMeshTwinBoundaryIncoherenceArrayName() ) );
  setSurfaceMeshTwinBoundaryArrayName(reader->readString("SurfaceMeshTwinBoundaryArrayName", getSurfaceMeshTwinBoundaryArrayName() ) );
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setAxisTolerance( reader->readValue("AxisTolerance", getAxisTolerance() ) );
  setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  setFindCoherence( reader->readValue("FindCoherence", getFindCoherence()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindTwinBoundaries::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshTwinBoundaryIncoherenceArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshTwinBoundaryArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceNormalsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(AxisTolerance)
  SIMPL_FILTER_WRITE_PARAMETER(AngleTolerance)
  SIMPL_FILTER_WRITE_PARAMETER(FindCoherence)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::dataCheckVoxel()
{
  setErrorCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getAvgQuatsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getAvgQuatsArrayPath()); }

  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeaturePhasesArrayPath()); }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::dataCheckSurfaceMesh()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getSurfaceMeshFaceLabelsArrayPath()); }

  if (getFindCoherence())
  {
    cDims[0] = 3;
    m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getSurfaceMeshFaceNormalsArrayPath()); }
  }

  cDims[0] = 1;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getSurfaceMeshFaceLabelsArrayPath().getAttributeMatrixName(), getSurfaceMeshTwinBoundaryArrayName() );
  m_SurfaceMeshTwinBoundaryPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTwinBoundaryPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTwinBoundary = m_SurfaceMeshTwinBoundaryPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (getFindCoherence())
  {
    tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getSurfaceMeshFaceLabelsArrayPath().getAttributeMatrixName(), getSurfaceMeshTwinBoundaryIncoherenceArrayName() );
    m_SurfaceMeshTwinBoundaryIncoherencePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 180.0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshTwinBoundaryIncoherencePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshTwinBoundaryIncoherence = m_SurfaceMeshTwinBoundaryIncoherencePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheckVoxel();
  dataCheckSurfaceMesh();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::execute()
{
  setErrorCondition(0);
  dataCheckVoxel();
  if(getErrorCondition() < 0) { return; }
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  size_t numTriangles = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  float angtol = m_AngleTolerance;
  float axistol = static_cast<float>( m_AxisTolerance * M_PI / 180.0f );

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateTwinBoundaryImpl(angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundaryIncoherence, m_FindCoherence), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateTwinBoundaryImpl serial(angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundaryIncoherence, m_FindCoherence);
    serial.generate(0, numTriangles);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTwinBoundaries::newFilterInstance(bool copyFilterParameters)
{
  FindTwinBoundaries::Pointer filter = FindTwinBoundaries::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographicFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getHumanLabel()
{ return "Find Twin Boundaries"; }
