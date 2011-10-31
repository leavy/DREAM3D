/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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


#ifndef _EbsdImporter_h_
#define _EbsdImporter_h_

#include "hdf5.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"

/**
 * @class EbsdImporter EbsdImporter.h EbsdLib/EbsdImporter.h
 * @brief  This class is a pure virtual class that defines the interface that
 * subclasses should implement in order to write a conforming EBSD Data Importer
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Aug 8, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT EbsdImporter
{
  public:
    EBSD_SHARED_POINTERS(EbsdImporter)
    EBSD_TYPE_MACRO(EbsdImporter)

    virtual ~EbsdImporter(){};

    /**
     * @brief Sets an Error Message
     */
    EBSD_VIRTUAL_INSTANCE_STRING_PROPERTY(ErrorMessage);

    /**
     * @brief Sets an error condition
     */
    EBSD_VIRTUAL_INSTANCE_PROPERTY(int, ErrorCondition);

    /**
     * @brief Cancel the operation
     */
    EBSD_VIRTUAL_INSTANCE_PROPERTY(bool, Cancel);

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the Reconstruction normalized to a value between 0 and 100
     */
    virtual void progressMessage(const std::string &message, int progress)
    {
      std::cout << progress << "% " << message << std::endl;
    }


    /**
     * @brief Imports the EBSD file. This is a pure virtual method to force subclasses
     * to implement this method.
     * @param fildId HDF5 fileId of an open HDF5 file that the data will be stored into
     * @param index The integer index value of this EBSD data file
     * @param ebsdFile The raw data file from the manufacturere (.ang, .ctf)
     */
    virtual int importFile(hid_t fileId, int index, const std::string &ebsd) = 0;

    /**
     * @brief Returns the dimensions for the EBSD Data set
     * @param x Number of X Voxels (out)
     * @param y Number of Y Voxels (out)
     */
    virtual void getDims(int &x, int &y) = 0;

    /**
     * @brief Returns the x and y resolution of the voxels
     * @param x The x resolution (out)
     * @param y The y resolution (out)
     */
    virtual void getResolution(float &x, float &y) = 0;


  protected:
    EbsdImporter() :
      m_ErrorCondition(0),
      m_Cancel(false)
    {
      m_ErrorMessage = "";
    }

  private:
    EbsdImporter(const EbsdImporter&); // Copy Constructor Not Implemented
    void operator=(const EbsdImporter&); // Operator '=' Not Implemented
};



#endif /* _EbsdImporter_h_  */
