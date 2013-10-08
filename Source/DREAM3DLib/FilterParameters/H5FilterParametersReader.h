/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _H5FilterParametersREADER_H_
#define _H5FilterParametersREADER_H_

#include "hdf5.h"

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"


/*
 *
 */
class DREAM3DLib_EXPORT H5FilterParametersReader : public AbstractFilterParametersReader
{
  public:
    DREAM3D_SHARED_POINTERS(H5FilterParametersReader)
    DREAM3D_STATIC_NEW_MACRO(H5FilterParametersReader)
    DREAM3D_TYPE_MACRO_SUPER(H5FilterParametersReader, AbstractFilterParametersReader)


    virtual ~H5FilterParametersReader();

    /**
     * @brief OpenDREAM3DFile This will open the HDF5 based DREAM3D file and open the proper HDF5 internal
     * group that is associated with the pipeline storage
     * @param filepath
     * @param fid The HDF5 file Id that represents the open HDF5 file
     * @return Return a pointer to a newly instantiated instance of this class. A Null Pointer will be returned if
     * the file can not be opened or the group is missing.
     */
    static Pointer OpenDREAM3DFileForReadingPipeline(QString filePath, hid_t &fid);


    DREAM3D_INSTANCE_PROPERTY(hid_t, PipelineGroupId)

    virtual int openFilterGroup(AbstractFilter* filter, int index);
    virtual int closeFilterGroup();

    virtual QString readString(const QString name, QString value);
    virtual QVector<QString> readStrings(const QString name, QVector<QString> value);

    virtual int8_t readValue(const QString name, int8_t value);
    virtual int16_t readValue(const QString name, int16_t value);
    virtual int32_t readValue(const QString name, int32_t value);
    virtual int64_t readValue(const QString name, int64_t value);
    virtual uint8_t readValue(const QString name, uint8_t value);
    virtual uint16_t readValue(const QString name, uint16_t value);
    virtual uint32_t readValue(const QString name, uint32_t value);
    virtual uint64_t readValue(const QString name, uint64_t value);
    virtual float readValue(const QString name, float value);
    virtual double readValue(const QString name, double value);
    virtual bool readValue(const QString name, bool value);

    virtual QVector<int8_t> readArray(const QString name, QVector<int8_t> value);
    virtual QVector<int16_t> readArray(const QString name, QVector<int16_t> value);
    virtual QVector<int32_t> readArray(const QString name, QVector<int32_t> value);
    virtual QVector<int64_t> readArray(const QString name, QVector<int64_t> value);
    virtual QVector<uint8_t> readArray(const QString name, QVector<uint8_t> value);
    virtual QVector<uint16_t> readArray(const QString name, QVector<uint16_t> value);
    virtual QVector<uint32_t> readArray(const QString name, QVector<uint32_t> value);
    virtual QVector<uint64_t> readArray(const QString name, QVector<uint64_t> value);
    virtual QVector<float> readArray(const QString name, QVector<float> value);
    virtual QVector<double> readArray(const QString name, QVector<double> value);


    virtual IntVec3Widget_t readIntVec3(const QString name, IntVec3Widget_t v);
    virtual FloatVec3Widget_t readFloatVec3(const QString name, FloatVec3Widget_t v);
    virtual ComparisonInput_t readComparisonInput(const QString name, ComparisonInput_t v, int vectorPos);
    virtual QVector<ComparisonInput_t> readComparisonInputs(const QString name, QVector<ComparisonInput_t> defValue);
    virtual AxisAngleInput_t readAxisAngle(const QString name, AxisAngleInput_t v, int vectorPos);
    virtual QVector<AxisAngleInput_t> readAxisAngles(const QString name, QVector<AxisAngleInput_t> defValue);
    virtual QSet<QString> readArraySelections(const QString name, QSet<QString> v);

  protected:
    H5FilterParametersReader();

  private:
    hid_t m_CurrentGroupId;

    H5FilterParametersReader(const H5FilterParametersReader&); // Copy Constructor Not Implemented
    void operator=(const H5FilterParametersReader&); // Operator '=' Not Implemented
};

#endif /* _H5FilterParametersREADER_H_ */