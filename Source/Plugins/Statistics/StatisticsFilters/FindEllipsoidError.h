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


#ifndef _FindEllipsoidError_H_
#define _FindEllipsoidError_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"

#include "Statistics/StatisticsConstants.h"

/**
 * @class FindEllipsoidError FindEllipsoidError.h DREAM3DLib/GenericFilters/FindEllipsoidError.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class FindEllipsoidError : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindEllipsoidError)
    DREAM3D_STATIC_NEW_MACRO(FindEllipsoidError)
    DREAM3D_TYPE_MACRO_SUPER(FindEllipsoidError, AbstractFilter)

    virtual ~FindEllipsoidError();
    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixName)
    Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
    Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, NumCellsArrayPath)
    Q_PROPERTY(DataArrayPath NumCellsArrayPath READ getNumCellsArrayPath WRITE setNumCellsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AxisLengthsArrayPath)
    Q_PROPERTY(DataArrayPath AxisLengthsArrayPath READ getAxisLengthsArrayPath WRITE setAxisLengthsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AxisEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath AxisEulerAnglesArrayPath READ getAxisEulerAnglesArrayPath WRITE setAxisEulerAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, IdealFeatureIdsArrayName)
    Q_PROPERTY(QString IdealFeatureIdsArrayName READ getIdealFeatureIdsArrayName WRITE setIdealFeatureIdsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, EllipsoidErrorArrayName)
    Q_PROPERTY(QString EllipsoidErrorArrayName READ getEllipsoidErrorArrayName WRITE setEllipsoidErrorArrayName)

    DREAM3D_FILTER_PARAMETER(bool, WriteIdealEllipseFeatureIds)
    Q_PROPERTY(bool WriteIdealEllipseFeatureIds READ getWriteIdealEllipseFeatureIds WRITE setWriteIdealEllipseFeatureIds)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  signals:
    /**
     * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
     * be pushed from a user-facing control (such as a widget)
     * @param filter Filter instance pointer
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged Emitted when any Filter parameter is changed internally
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before calling dataCheck()
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
    void preflightExecuted();

  protected:
    FindEllipsoidError();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    void find_error2D();

  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_DATAARRAY_VARIABLE(float, AxisEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(float, Centroids)
    DEFINE_DATAARRAY_VARIABLE(float, AxisLengths)
    DEFINE_DATAARRAY_VARIABLE(int32_t, NumCells)
    DEFINE_DATAARRAY_VARIABLE(int32_t, IdealFeatureIds)
    DEFINE_DATAARRAY_VARIABLE(float, EllipsoidError)

    double scaleFactor;

    FindEllipsoidError(const FindEllipsoidError&); // Copy Constructor Not Implemented
    void operator=(const FindEllipsoidError&); // Operator '=' Not Implemented
};

#endif /* FindEllipsoidError_H_ */
