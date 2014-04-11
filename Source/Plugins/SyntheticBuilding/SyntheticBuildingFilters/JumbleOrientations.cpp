/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "JumbleOrientations.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/Math/OrientationMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

JumbleOrientations::JumbleOrientations() :
  AbstractFilter(),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_CellEulerAnglesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::EulerAngles),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_FeatureEulerAngles(NULL),
  m_AvgQuats(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JumbleOrientations::~JumbleOrientations()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Select Synthetic Volume DataContainer", "DataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget,"QString", false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("CellEulerAngles", "CellEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("FeatureEulerAngles", "FeatureEulerAnglesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAvgQuatsArrayName(reader->readString("AvgQuatsArrayName", getAvgQuatsArrayName() ) );
  setFeatureEulerAnglesArrayName(reader->readString("FeatureEulerAnglesArrayName", getFeatureEulerAnglesArrayName() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JumbleOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("AvgQuatsArrayName", getAvgQuatsArrayName() );
  writer->writeValue("FeatureEulerAnglesArrayName", getFeatureEulerAnglesArrayName() );
  writer->writeValue("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() );
  writer->writeValue("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this,  getCellEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this,  getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getFeatureEulerAnglesArrayName() );
  m_FeatureEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getAvgQuatsArrayName() );
  m_AvgQuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  int64_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // Generate all the numbers up front
  const int rangeMin = 1;
  const int rangeMax = totalFeatures - 1;
  typedef boost::uniform_int<int> NumberDistribution;
  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::variate_generator < RandomNumberGenerator&,
          NumberDistribution > Generator;

  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(static_cast<boost::uint32_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time

  int r;
  float temp1, temp2, temp3;
  //--- Shuffle elements by randomly exchanging each with one other.
  for (int i = 1; i < totalFeatures; i++)
  {
    bool good = false;
    while(good == false)
    {
      good = true;
      r = numberGenerator(); // Random remaining position.
      if (r >= totalFeatures) { good = false; }
      if (m_FeaturePhases[i] != m_FeaturePhases[r]) { good = false; }
    }
    temp1 = m_FeatureEulerAngles[3 * i];
    temp2 = m_FeatureEulerAngles[3 * i + 1];
    temp3 = m_FeatureEulerAngles[3 * i + 2];
    m_FeatureEulerAngles[3 * i] = m_FeatureEulerAngles[3 * r];
    m_FeatureEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * r + 1];
    m_FeatureEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * r + 2];
    m_FeatureEulerAngles[3 * r] = temp1;
    m_FeatureEulerAngles[3 * r + 1] = temp2;
    m_FeatureEulerAngles[3 * r + 2] = temp3;
  }

  // Now adjust all the Euler angle values for each Voxel
  for(int64_t i = 0; i < totalPoints; ++i)
  {
    m_CellEulerAngles[3 * i] = m_FeatureEulerAngles[3 * (m_FeatureIds[i])];
    m_CellEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * (m_FeatureIds[i]) + 1];
    m_CellEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * (m_FeatureIds[i]) + 2];
  }
  QuatF q;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  for (int i = 1; i < totalFeatures; i++)
  {
    OrientationMath::EulertoQuat(m_FeatureEulerAngles[3 * i], m_FeatureEulerAngles[3 * i + 1], m_FeatureEulerAngles[3 * i + 2], q);
    QuaternionMathF::Copy(q, avgQuats[i]);
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Jumbling Orientations Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer JumbleOrientations::newFilterInstance(bool copyFilterParameters)
{
  JumbleOrientations::Pointer filter = JumbleOrientations::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    
    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                              " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                              " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                              .arg(parameter->getPropertyName())
                              .arg(filter->getHumanLabel())
                              .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}