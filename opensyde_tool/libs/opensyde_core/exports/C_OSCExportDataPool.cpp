//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Export data pool settings of a openSYDE node.

   Create a .c and .h file providing entire data pool configuration.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.08.2017  STW/U.Roesch
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include "stwtypes.h"
#include "stwerrors.h"

#include "CSCLStringList.h"
#include "C_OSCExportDataPool.h"
#include "C_OSCNodeDataPoolContent.h"
#include "C_OSCUtils.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Constructor

   Initialize all elements with default values

   \created     23.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
C_OSCExportDataPool::C_OSCExportDataPool(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     23.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
C_OSCExportDataPool::~C_OSCExportDataPool(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Return filename (without extension)

   The caller must provide a valid data pool.

   \param[in]  orc_DataPool            data pool configuration
   \param[out] orc_FileName            assembled filename

   \created     17.10.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
void C_OSCExportDataPool::h_GetFileName(const C_OSCNodeDataPool & orc_DataPool, C_SCLString & orc_FileName)
{
   mh_CreateFileName(orc_DataPool.c_Name, orc_FileName);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create source files

   The caller must provide a valid path and data pool.

   \param[in] orc_Path                 storage path for created files
   \param[in] orc_DataPool             data pool configuration
   \param[in] ou8_DataPoolIndex        index of data pool within local process
   \param[in] oq_IsRemote              true: create source code for a "remote" data pool
   \param[in] ou8_DataPoolIndexRemote  index of data pool within remote process
   \param[in] ou8_ProcessId            ID of process owning this data pool

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store files

   \created     23.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportDataPool::h_CreateSourceCode(const C_SCLString & orc_Path, const C_OSCNodeDataPool & orc_DataPool,
                                               const uint8 ou8_DataPoolIndex, const bool oq_IsRemote,
                                               const uint8 ou8_DataPoolIndexRemote, const uint8 ou8_ProcessId)
{
   sint32 s32_Retval;
   uint32 u32_HashValue = 0U;

   //calculate hash value over the current state of the data pool definition
   orc_DataPool.CalcHash(u32_HashValue);
   const C_SCLString c_ProjectId = C_SCLString::IntToStr(u32_HashValue);

   // create header file
   s32_Retval = mh_CreateHeaderFile(orc_Path, orc_DataPool, ou8_DataPoolIndex, c_ProjectId, oq_IsRemote);

   // create implementation file
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CreateImplementationFile(orc_Path, orc_DataPool, ou8_DataPoolIndex, c_ProjectId,
                                               ou8_DataPoolIndexRemote, ou8_ProcessId, oq_IsRemote);
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create header file

   \param[in] orc_Path                 storage path for created file
   \param[in] orc_DataPool             data pool configuration
   \param[in] ou8_DataPoolIndex        index of data pool
   \param[in] orc_ProjectId            project id for consistency check
   \param[in] oq_IsRemote              true: create for remote data pool

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store file

   \created     24.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportDataPool::mh_CreateHeaderFile(const C_SCLString & orc_Path, const C_OSCNodeDataPool & orc_DataPool,
                                                const uint8 ou8_DataPoolIndex, const C_SCLString & orc_ProjectId,
                                                const bool oq_IsRemote)
{
   sint32 s32_Retval;
   C_SCLStringList c_Data;

   // add header
   mh_AddHeader(c_Data, orc_DataPool, mhq_IS_HEADER_FILE);

   // add includes
   mh_AddIncludes(c_Data, orc_DataPool, mhq_IS_HEADER_FILE);

   // add defines
   mh_AddDefines(c_Data, orc_DataPool, ou8_DataPoolIndex, orc_ProjectId, mhq_IS_HEADER_FILE, oq_IsRemote);

   // add types
   mh_AddTypes(c_Data, orc_DataPool, mhq_IS_HEADER_FILE, oq_IsRemote);

   // add global variables
   mh_AddGlobalVariables(c_Data, orc_DataPool, mhq_IS_HEADER_FILE, oq_IsRemote);

   // add function prototypes
   c_Data.Append(
      "/* -- Function Prototypes ------------------------------------------------------------------------------------------- */");
   c_Data.Append("");

   // add implementation
   mh_AddImplementation(c_Data, mhq_IS_HEADER_FILE);

   // finally save all stuff into the file
   s32_Retval = mh_SaveToFile(c_Data, orc_Path, orc_DataPool.c_Name, mhq_IS_HEADER_FILE);

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create implementation file

   \param[in] orc_Path                 storage path for created file
   \param[in] orc_DataPool             data pool configuration
   \param[in] ou8_DataPoolIndex        index of data pool
   \param[in] orc_ProjectId            project id for consistency check
   \param[in] ou8_DataPoolIndexRemote  index of data pool within remote process
   \param[in] ou8_ProcessId            ID of process owning this data pool
   \param[in] oq_IsRemote              true: create for remote data pool

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store file

   \created     24.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportDataPool::mh_CreateImplementationFile(const C_SCLString & orc_Path,
                                                        const C_OSCNodeDataPool & orc_DataPool,
                                                        const uint8 ou8_DataPoolIndex,
                                                        const C_SCLString & orc_ProjectId,
                                                        const uint8 ou8_DataPoolIndexRemote, const uint8 ou8_ProcessId,
                                                        const bool oq_IsRemote)
{
   sint32 s32_Retval;
   C_SCLStringList c_Data;

   // add header
   mh_AddHeader(c_Data, orc_DataPool, mhq_IS_IMPLEMENTATION_FILE);

   // add includes
   mh_AddIncludes(c_Data, orc_DataPool, mhq_IS_IMPLEMENTATION_FILE);

   // add defines
   mh_AddDefines(c_Data, orc_DataPool, ou8_DataPoolIndex, orc_ProjectId, mhq_IS_IMPLEMENTATION_FILE, oq_IsRemote);

   // add types
   mh_AddTypes(c_Data, orc_DataPool, mhq_IS_IMPLEMENTATION_FILE, oq_IsRemote);

   // add global variables
   mh_AddGlobalVariables(c_Data, orc_DataPool, mhq_IS_IMPLEMENTATION_FILE, oq_IsRemote);

   // add module global variables and function prototypes
   mh_AddModuleGlobal(c_Data, orc_DataPool, ou8_ProcessId, ou8_DataPoolIndexRemote, oq_IsRemote);

   // add implementation
   mh_AddImplementation(c_Data, mhq_IS_IMPLEMENTATION_FILE);

   // finally save all stuff into the file
   s32_Retval = mh_SaveToFile(c_Data, orc_Path, orc_DataPool.c_Name, mhq_IS_IMPLEMENTATION_FILE);

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add file header

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             data pool configuration
   \param[in]  oq_FileType              .c or .h file selected

   \created     24.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddHeader(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                       const bool oq_FileType)
{
   orc_Data.Append(
      "//----------------------------------------------------------------------------------------------------------------------");
   orc_Data.Append("/*!");
   orc_Data.Append("   \\file");
   if (oq_FileType == mhq_IS_IMPLEMENTATION_FILE)
   {
      orc_Data.Append("   \\brief       openSYDE Data Pool definition (Source file with constant definitions)");
      orc_Data.Append("");
      orc_Data.Append("   " + C_OSCUtils::h_NiceifyStringForCComment(orc_DataPool.c_Comment));
   }
   else
   {
      orc_Data.Append(
         "   \\brief       openSYDE Data Pool definition (Header file with constant and global definitions)");
   }
   orc_Data.Append("*/");
   orc_Data.Append(
      "//----------------------------------------------------------------------------------------------------------------------");

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      const C_SCLString c_DataPoolName = orc_DataPool.c_Name.UpperCase();
      orc_Data.Append("#ifndef " + c_DataPoolName + "_DATA_POOLH");
      orc_Data.Append("#define " + c_DataPoolName + "_DATA_POOLH");
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add includes

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             data pool configuration
   \param[in]  oq_FileType              .c or .h file selected

   \created     24.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddIncludes(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                         const bool oq_FileType)
{
   orc_Data.Append("");
   orc_Data.Append(
      "/* -- Includes ------------------------------------------------------------------------------------------------------ */");

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("#include \"stwtypes.h\"");
      orc_Data.Append("#include \"osy_dpa_data_pool.h\"");
      orc_Data.Append("");
      orc_Data.Append("#ifdef __cplusplus");
      orc_Data.Append("extern \"C\" {");
      orc_Data.Append("#endif");
      orc_Data.Append("");
   }
   else
   {
      const C_SCLString c_DataPoolName = orc_DataPool.c_Name.LowerCase();
      orc_Data.Append("#include <stddef.h>");
      orc_Data.Append("#include \"" + c_DataPoolName + "_data_pool.h\"");
      orc_Data.Append("");
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add defines

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             data pool configuration
   \param[in]  ou8_DataPoolIndex        index of data pool
   \param[in]  orc_ProjectId            project id for consistency check
   \param[in]  oq_FileType              .c or .h file selected
   \param[in]  oq_IsRemote              true: create for remote data pool

   \created     24.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddDefines(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                        const uint8 ou8_DataPoolIndex, const C_SCLString & orc_ProjectId,
                                        const bool oq_FileType, const bool oq_IsRemote)
{
   const C_SCLString c_DataPoolName = orc_DataPool.c_Name.UpperCase();
   const C_SCLString c_MagicName = c_DataPoolName + "_PROJECT_ID_" + orc_ProjectId;
   uint16 u16_ListIndex;

   orc_Data.Append(
      "/* -- Defines ------------------------------------------------------------------------------------------------------- */");

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("///unique ID to ensure consistency between .h and .c files");
      orc_Data.Append("#define " + c_MagicName + " void " + c_MagicName.LowerCase() + "(void) {}");
      orc_Data.Append("");
      orc_Data.Append("///Index of this data pool");
      orc_Data.Append("#define " + c_DataPoolName + "_DATA_POOL_INDEX (" + C_SCLString::IntToStr(ou8_DataPoolIndex) +
                      "U)");
      orc_Data.Append("");

      if (oq_IsRemote == false)
      {
         orc_Data.Append("///Index of lists");
         for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
         {
            const C_SCLString c_ListName = orc_DataPool.c_Lists[u16_ListIndex].c_Name.UpperCase();
            orc_Data.Append("#define " + c_DataPoolName + "_LIST_INDEX_" + c_ListName + " (" +
                            C_SCLString::IntToStr(u16_ListIndex) + "U)");
         }
         orc_Data.Append("#define " + c_DataPoolName + "_NUMBER_OF_LISTS " + "(" + C_SCLString::IntToStr(
                            u16_ListIndex) + "U)");
         orc_Data.Append("");

         orc_Data.Append("///Index of elements");
         for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
         {
            const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
            const C_SCLString c_ListName = rc_List.c_Name.UpperCase();
            for (uint16 u16_ElementIndex = 0U; u16_ElementIndex < rc_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_SCLString c_ElementName = rc_List.c_Elements[u16_ElementIndex].c_Name.UpperCase();
               orc_Data.Append("#define " + c_DataPoolName + "_ELEM_INDEX_" + c_ListName + "_" + c_ElementName +
                               " (" + C_SCLString::IntToStr(u16_ElementIndex) + "U)");
            }
            orc_Data.Append("#define " + c_DataPoolName + "_" + c_ListName + "_NUMBER_OF_ELEMENTS " + "(" +
                            C_SCLString::IntToStr(rc_List.c_Elements.size()) + "U)");
            orc_Data.Append("");
         }

         orc_Data.Append("///Index of data sets");
         for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
         {
            const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
            const C_SCLString c_ListName = rc_List.c_Name.UpperCase();
            if (rc_List.c_Elements.size() > 0)
            {
               for (uint16 u16_DataSetIndex = 0U; u16_DataSetIndex < rc_List.c_DataSets.size(); u16_DataSetIndex++)
               {
                  const C_SCLString c_DataSetName = rc_List.c_DataSets[u16_DataSetIndex].c_Name.UpperCase();
                  orc_Data.Append("#define " + c_DataPoolName + "_DATA_SET_INDEX_" + c_ListName + "_" + c_DataSetName +
                                  " (" + C_SCLString::IntToStr(u16_DataSetIndex) + "U)");
               }
               orc_Data.Append("#define " + c_DataPoolName + "_" + c_ListName + "_NUMBER_OF_DATA_SETS " + "(" +
                               C_SCLString::IntToStr(rc_List.c_DataSets.size()) + "U)");
            }
            else
            {
               orc_Data.Append("#define " + c_DataPoolName + "_" + c_ListName + "_NUMBER_OF_DATA_SETS " + "(0U)");
            }
            orc_Data.Append("");
         }
      }
   }
   else
   {
      orc_Data.Append("///ensure file consistency (if compilation fails here the .h file does not match this .c file)");
      orc_Data.Append(c_MagicName);
      orc_Data.Append("");
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add types

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             data pool configuration
   \param[in]  oq_FileType              .c or .h file selected
   \param[in]  oq_IsRemote              true: create for remote data pool

   \created     28.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddTypes(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                      const bool oq_FileType, const bool oq_IsRemote)
{
   orc_Data.Append(
      "/* -- Types --------------------------------------------------------------------------------------------------------- */");

   if ((oq_FileType == mhq_IS_HEADER_FILE) && (oq_IsRemote == false))
   {
      bool q_AtLeastOneElement = false;

      for (uint16 u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
         if (rc_List.c_Elements.size() != 0)
         {
            q_AtLeastOneElement = true;
            orc_Data.Append("///Elements in list \"" + rc_List.c_Name + "\":");
            orc_Data.Append("typedef struct");
            orc_Data.Append("{");

            for (uint16 u16_ElementIndex = 0U; u16_ElementIndex < rc_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = rc_List.c_Elements[u16_ElementIndex];
               const C_SCLString c_TypePrefix = mh_GetTypePrefix(rc_Element.GetType(), rc_Element.GetArray());
               const C_SCLString c_ElementType =  mh_GetType(rc_Element.GetType());
               if (rc_Element.GetArray() == true)
               {
                  const C_SCLString c_String = C_SCLString::IntToStr(rc_Element.GetArraySize());
                  orc_Data.Append("   " + c_ElementType + " " + c_TypePrefix + "_" + rc_Element.c_Name + "[" +
                                  c_String + "]; /* " + C_OSCUtils::h_NiceifyStringForCComment(rc_Element.c_Comment) +
                                  " */");
               }
               else
               {
                  orc_Data.Append("   " + c_ElementType + " " + c_TypePrefix + "_" + rc_Element.c_Name + "; /* " +
                                  C_OSCUtils::h_NiceifyStringForCComment(rc_Element.c_Comment) + " */");
               }
            }
            orc_Data.Append("} T_" + orc_DataPool.c_Name + "_" + rc_List.c_Name + "_Values;");
            orc_Data.Append("");
         }
      }

      if (q_AtLeastOneElement == true)
      {
         orc_Data.Append("///Elements of all lists:");
         orc_Data.Append("typedef struct");
         orc_Data.Append("{");

         for (uint16 u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
         {
            if (orc_DataPool.c_Lists[u16_ListIndex].c_Elements.size() != 0)
            {
               const C_SCLString c_ListName = orc_DataPool.c_Lists[u16_ListIndex].c_Name;
               orc_Data.Append("   T_" + orc_DataPool.c_Name + "_" + c_ListName + "_Values t_" + c_ListName +
                               "Values;");
            }
         }

         orc_Data.Append("} T_" + orc_DataPool.c_Name + "_DataPoolValues;");
      }
   }
   orc_Data.Append("");
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add global variables

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             data pool configuration
   \param[in]  oq_FileType              .c or .h file selected
   \param[in]  oq_IsRemote              true: create for remote data pool

   \created     28.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddGlobalVariables(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                                const bool oq_FileType, const bool oq_IsRemote)
{
   bool q_AtLeastOneElement = false;

   for (uint16 u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
   {
      if (orc_DataPool.c_Lists[u16_ListIndex].c_Elements.size() != 0)
      {
         q_AtLeastOneElement = true;
         break;
      }
   }
   orc_Data.Append(
      "/* -- Global Variables ---------------------------------------------------------------------------------------------- */");

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      if (oq_IsRemote == false)
      {
         if (q_AtLeastOneElement == true)
         {
            orc_Data.Append("///Global data pool RAM variables:");
            orc_Data.Append("extern T_" + orc_DataPool.c_Name + "_DataPoolValues " + "gt_" + orc_DataPool.c_Name +
                            "_DataPoolValues;");
            orc_Data.Append("");
         }
      }

      orc_Data.Append("///Data pool instance data for API functions::");
      orc_Data.Append("extern const T_osy_dpa_data_pool gt_" + orc_DataPool.c_Name + "_DataPool;");
      orc_Data.Append("");
   }
   else
   {
      if (oq_IsRemote == false)
      {
         if (q_AtLeastOneElement == true)
         {
            C_SCLString c_String;
            if (orc_DataPool.q_IsSafety == true)
            {
               c_String = "OSY_DPA_SAFE_RAM_DATA ";
            }
            orc_Data.Append(c_String + "T_" + orc_DataPool.c_Name + "_DataPoolValues gt_" + orc_DataPool.c_Name +
                            "_DataPoolValues;");
         }
      }

      orc_Data.Append("");
      orc_Data.Append("");
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add module global variables and function prototypes

   Implementation file only.

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             data pool configuration
   \param[in]  ou8_ProcessId            ID of process owning this data pool
   \param[in]  ou8_DataPoolIndexRemote  index of data pool within remote process (only used for remote data pools)
   \param[in]  oq_IsRemote              true: create for remote data pool

   \created     29.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddModuleGlobal(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                             const uint8 ou8_ProcessId, const uint8 ou8_DataPoolIndexRemote,
                                             const bool oq_IsRemote)
{
   C_SCLString c_DataPoolName = orc_DataPool.c_Name;
   C_SCLString c_String;

   orc_Data.Append(
      "/* -- Module Global Variables --------------------------------------------------------------------------------------- */\n");
   if (oq_IsRemote == false)
   {
      uint16 u16_ListIndex;
      uint16 u16_ElementIndex;
      uint32 u32_HashValue = 0U;
      C_SCLString c_ListName;

      orc_Data.Append("//Create data pool definition instance data:");
      orc_Data.Append("OSY_DPA_CREATE_STATIC_DP_DEFINITION_INSTANCE_DATA(mt_DpDefinitionInstanceData)\n");

      orc_Data.Append("///Min/max values:");

      for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         C_OSCNodeDataPoolList c_List = orc_DataPool.c_Lists[u16_ListIndex];
         if (c_List.c_Elements.size() != 0)
         {
            orc_Data.Append("///Minimum values");
            orc_Data.Append("static const T_" + orc_DataPool.c_Name + "_" + c_List.c_Name + "_Values mt_" +
                            c_List.c_Name + "MinValues =\n{");
            for (u16_ElementIndex = 0U; u16_ElementIndex < c_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = c_List.c_Elements[u16_ElementIndex];
               c_String = "   " + mh_GetElementValueString(rc_Element.c_MinValue,
                                                           rc_Element.GetType(), rc_Element.GetArray());
               if (u16_ElementIndex == (c_List.c_Elements.size() - 1U))
               {
                  c_String += " ";
               }
               else
               {
                  c_String += ",";
               }
               c_String += ("   ///< " + rc_Element.c_Name + " (" +
                            C_OSCUtils::h_NiceifyStringForCComment(rc_Element.c_Comment) + ")");
               orc_Data.Append(c_String);
            }
            orc_Data.Append("};\n");

            orc_Data.Append("///Maximum values");
            orc_Data.Append("static const T_" + orc_DataPool.c_Name + "_" + c_List.c_Name + "_Values mt_" + c_List.c_Name +
                            "MaxValues =\n{");
            for (u16_ElementIndex = 0U; u16_ElementIndex < c_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = c_List.c_Elements[u16_ElementIndex];
               c_String = "   " + mh_GetElementValueString(rc_Element.c_MaxValue,
                                                           rc_Element.GetType(), rc_Element.GetArray());
               if (u16_ElementIndex == (c_List.c_Elements.size() - 1U))
               {
                  c_String += " ";
               }
               else
               {
                  c_String += ",";
               }
               c_String += ("   ///< " + rc_Element.c_Name + " (" +
                            C_OSCUtils::h_NiceifyStringForCComment(rc_Element.c_Comment) + ")");
               orc_Data.Append(c_String);
            }
            orc_Data.Append("};\n");

            if (c_List.c_DataSets.size() > 0)
            {
               uint8 u8_DataSetIndex;
               orc_Data.Append("///Data set values");
               c_ListName = c_List.c_Name;
               orc_Data.Append("static const T_" + c_DataPoolName + "_" + c_List.c_Name + "_Values mat_" + c_List.c_Name +
                               "DataSetValues [" + c_DataPoolName.UpperCase() + "_" + c_ListName.UpperCase() +
                               "_NUMBER_OF_DATA_SETS] =\n{");
               for (u8_DataSetIndex = 0U; u8_DataSetIndex < c_List.c_DataSets.size(); u8_DataSetIndex++)
               {
                  orc_Data.Append("   {");
                  for (u16_ElementIndex = 0U; u16_ElementIndex < c_List.c_Elements.size(); u16_ElementIndex++)
                  {
                     C_OSCNodeDataPoolListElement c_Element = c_List.c_Elements[u16_ElementIndex];
                     c_String = "      ";
                     c_String += mh_GetElementValueString(c_Element.c_DataSetValues[u8_DataSetIndex],
                                                          c_Element.GetType(),
                                                          c_Element.GetArray());
                     if (u16_ElementIndex == (c_List.c_Elements.size() - 1U))
                     {
                        c_String += " ";
                     }
                     else
                     {
                        c_String += ",";
                     }
                     c_String += ("   ///< " + c_Element.c_Name + " (" +
                                  C_OSCUtils::h_NiceifyStringForCComment(c_Element.c_Comment) + ")");
                     orc_Data.Append(c_String);
                  }
                  if (u8_DataSetIndex == (c_List.c_DataSets.size() - 1U))
                  {
                     orc_Data.Append("   }");
                  }
                  else
                  {
                     orc_Data.Append("   },");
                  }
               }
               orc_Data.Append("};\n");

               orc_Data.Append("///Data set table:");
               c_ListName = c_List.c_Name;
               orc_Data.Append("static const T_osy_dpa_data_set mat_" + c_List.c_Name + "DataSetTable[" +
                               c_DataPoolName.UpperCase() + "_" +  c_ListName.UpperCase() +
                               "_NUMBER_OF_DATA_SETS] =\n{");
               for (u8_DataSetIndex = 0U; u8_DataSetIndex < c_List.c_DataSets.size(); u8_DataSetIndex++)
               {
                  c_String = "   { &mat_" + c_List.c_Name + "DataSetValues[" + C_SCLString::IntToStr(u8_DataSetIndex) +
                             "] }";

                  if (u8_DataSetIndex != (c_List.c_DataSets.size() - 1U))
                  {
                     c_String += ",";
                  }
                  orc_Data.Append(c_String);
               }
               orc_Data.Append("};");
               orc_Data.Append("");
            }
         }
      }

      orc_Data.Append("///List definitions:");
      for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
         if (rc_List.c_Elements.size() != 0)
         {
            c_ListName = rc_List.c_Name;
            orc_Data.Append("static const T_osy_dpa_element_definition mat_DataPool" + rc_List.c_Name + "Elements[" +
                            c_DataPoolName.UpperCase() + "_" + c_ListName.UpperCase() + "_NUMBER_OF_ELEMENTS] =");
            orc_Data.Append("{");

            for (u16_ElementIndex = 0U; u16_ElementIndex < rc_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = rc_List.c_Elements[u16_ElementIndex];
               const C_SCLString c_ElementSize = mh_GetElementSize(rc_Element.GetType(), rc_Element.GetArraySize(),
                                                                   rc_Element.GetArray());
               const C_SCLString c_TypePrefix = mh_GetTypePrefix(rc_Element.GetType(), rc_Element.GetArray());
               const C_SCLString c_TriggerEvent = (rc_Element.q_DiagEventCall == true) ? "1U" : "0U";
               C_SCLString c_ElementName = c_TypePrefix + "_" + rc_Element.c_Name;
               C_SCLString c_ElementType = mh_GetType(rc_Element.GetType());
               if (rc_Element.GetArray() == true)
               {
                  c_ElementName += "[0]";
                  c_ElementType = "a" + c_ElementType;
               }
               c_String = "   { OSY_DPA_ELEMENT_TYPE_" + c_ElementType.UpperCase() + ", " + c_TriggerEvent + ", " +
                          c_ElementSize + "U, &gt_" + c_DataPoolName + "_DataPoolValues.t_" + c_ListName + "Values." +
                          c_ElementName +
                          ", &mt_" + c_ListName + "MinValues." + c_ElementName +
                          ", &mt_" + c_ListName + "MaxValues." + c_ElementName + " }";
               if (u16_ElementIndex != (rc_List.c_Elements.size() - 1U))
               {
                  c_String += ",";
               }

               orc_Data.Append(c_String);
            }
            orc_Data.Append("};");
            orc_Data.Append("");
         }
      }

      orc_Data.Append("///list of lists:");
      orc_Data.Append("static const T_osy_dpa_list_definition mat_DataPoolLists[" + c_DataPoolName.UpperCase() +
                      "_NUMBER_OF_LISTS] =");
      orc_Data.Append("{");

      for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
         c_ListName = rc_List.c_Name;
         c_String = "   { ";
         c_String += c_DataPoolName.UpperCase() + "_" + c_ListName.UpperCase() + "_NUMBER_OF_ELEMENTS, ";
         c_String += c_DataPoolName.UpperCase() + "_" + c_ListName.UpperCase() + "_NUMBER_OF_DATA_SETS, ";

         if (rc_List.c_Elements.size() == 0)
         {
            //empty list with no elements
            //no CRC, no address, no size, pointers all NULL
            c_String += "0U, 0U, 0U, NULL, NULL, NULL";
         }
         else
         {
            if (orc_DataPool.e_Type == C_OSCNodeDataPool::eNVM)
            {
               c_String += C_SCLString::IntToStr(rc_List.q_NvMCRCActive) + "U, ";
               c_String += "0x" + C_SCLString::IntToHex(static_cast<sint64>(rc_List.u32_NvMStartAddress), 8U) + "U, ";
            }
            else
            {
               c_String += "0U, 0x00000000U, "; //no CRC and NVM start address
            }
            c_String += "sizeof(T_" + c_DataPoolName + "_" + c_ListName + "_Values), ";
            c_String += "&mat_DataPool" + c_ListName + "Elements[0], ";
            c_String += "&gt_" + c_DataPoolName + "_DataPoolValues.t_" + c_ListName + "Values, ";
            if (rc_List.c_DataSets.size() > 0U)
            {
               c_String += "&mat_" + c_ListName + "DataSetTable[0]";
            }
            else
            {
               c_String += "NULL";
            }
         }
         if (u16_ListIndex == (orc_DataPool.c_Lists.size() - 1U))
         {
            c_String += " }";
         }
         else
         {
            c_String += " },";
         }
         orc_Data.Append(c_String);
      }
      orc_Data.Append("};");
      orc_Data.Append("");

      orc_Data.Append("///Data pool definition:");
      orc_Data.Append("static const T_osy_dpa_data_pool_definition mt_DataPoolDefinition =");
      orc_Data.Append("{");
      orc_Data.Append("   OSY_DPA_DATA_POOL_DEFINITION_VERSION,");
      if (orc_DataPool.e_Type == C_OSCNodeDataPool::eNVM)
      {
         orc_Data.Append("   OSY_DPA_DATA_POOL_TYPE_NVM,");
      }
      else if (orc_DataPool.e_Type == C_OSCNodeDataPool::eCOM)
      {
         orc_Data.Append("   OSY_DPA_DATA_POOL_TYPE_COMMUNICATION,");
      }
      else
      {
         orc_Data.Append("   OSY_DPA_DATA_POOL_TYPE_DIAGNOSIS,");
      }
      orc_Data.Append("   { 0x" + C_SCLString::IntToHex(orc_DataPool.au8_Version[0], 2U) + "U, 0x" +
                      C_SCLString::IntToHex(orc_DataPool.au8_Version[1], 2U) + "U, 0x" +
                      C_SCLString::IntToHex(orc_DataPool.au8_Version[2], 2U) + "U },");
      orc_Data.Append("   \"" + orc_DataPool.c_Name + "\",");
      orc_Data.Append("   " + c_DataPoolName.UpperCase() + "_NUMBER_OF_LISTS,");
      orc_DataPool.CalcDefinitionHash(u32_HashValue);
      orc_Data.Append("   0x" + C_SCLString::IntToHex(static_cast<sint64>(u32_HashValue), 4U) + "U,");
      if (orc_DataPool.e_Type == C_OSCNodeDataPool::eNVM)
      {
         orc_Data.Append("   0x" + C_SCLString::IntToHex(static_cast<sint64>(orc_DataPool.u32_NvMStartAddress), 8U) +
                         "U,  ///< NVM start address");
         orc_Data.Append("   " + C_SCLString::IntToStr(orc_DataPool.u32_NvMSize) +
                         "U,  ///< number of bytes occupied in NVM");
      }
      else
      {
         orc_Data.Append("   0x00000000U,  ///< NVM start address");
         orc_Data.Append("   0U,  ///< number of bytes occupied in NVM");
      }
      orc_Data.Append("   &mat_DataPoolLists[0],");
      orc_Data.Append("   &mt_DpDefinitionInstanceData");
      orc_Data.Append("};");
      orc_Data.Append("");
   }

   if (oq_IsRemote == true)
   {
      c_String = "///Information about remote data pool.";
      c_String += "Can be used to request information about remote data pool from another process:";
      orc_Data.Append(c_String);
      orc_Data.Append("static const T_osy_dpa_remote_data_pool_info mt_RemoteDataPoolInfo =");
      orc_Data.Append("{");
      orc_Data.Append("   " + C_SCLString::IntToStr(ou8_ProcessId) +
                      "U, ///< Identification ID of remote server providing information about the data pool");
      orc_Data.Append("   " + C_SCLString::IntToStr(ou8_DataPoolIndexRemote) +
                      "U  ///< Index of data pool within process identified by ProcessId");
   }
   else
   {
      orc_Data.Append("///Information about process and data pool:");
      orc_Data.Append("static const T_osy_dpa_remote_data_pool_info mt_DataPoolInfo =");
      orc_Data.Append("{");
      orc_Data.Append("   " + C_SCLString::IntToStr(ou8_ProcessId) +
                      "U, ///< Identification ID of our own process");
      orc_Data.Append("   " + c_DataPoolName.UpperCase() + "_DATA_POOL_INDEX " +
                      "///< Index of data pool within process identified by ProcessId");
   }
   orc_Data.Append("};");
   orc_Data.Append("");

   orc_Data.Append("///Create data pool instance data:");
   if (oq_IsRemote == true)
   {
      orc_Data.Append("OSY_DPA_CREATE_STATIC_DP_INSTANCE_DATA(mt_DpInstanceData, NULL)");
   }
   else
   {
      orc_Data.Append("OSY_DPA_CREATE_STATIC_DP_INSTANCE_DATA(mt_DpInstanceData, &mt_DataPoolDefinition)");
   }
   orc_Data.Append("");

   orc_Data.Append("const T_osy_dpa_data_pool gt_" + c_DataPoolName + "_DataPool =");
   orc_Data.Append("{");
   orc_Data.Append("   OSY_DPA_DATA_POOL_MAGIC,  ///< identification of valid DP definition");
   orc_Data.Append("   " + c_DataPoolName.UpperCase() +
                   "_DATA_POOL_INDEX,  ///< data pool index within this process");
   if (oq_IsRemote == true)
   {
      orc_Data.Append("   1U,  ///< it's remote ...");
      orc_Data.Append("   &mt_RemoteDataPoolInfo,  ///< details about remote data pool");
   }
   else
   {
      orc_Data.Append("   0U,  ///< it's local ...");
      orc_Data.Append("   &mt_DataPoolInfo,  ///< info about our local data pool");
   }
   orc_Data.Append("   &mt_DpInstanceData  ///< run-time data");
   orc_Data.Append("};");
   orc_Data.Append("");

   orc_Data.Append(
      "/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */");
   orc_Data.Append("");
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add implementation

   \param[out] orc_Data                 converted data to string list
   \param[in]  oq_FileType              .c or .h file selected

   \created     28.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddImplementation(C_SCLStringList & orc_Data, const bool oq_FileType)
{
   orc_Data.Append(
      "/* -- Implementation ------------------------------------------------------------------------------------------------ */");
   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("#ifdef __cplusplus");
      orc_Data.Append("} /* end of extern \"C\" */");
      orc_Data.Append("#endif");
      orc_Data.Append("");
      orc_Data.Append("#endif");
   }
   else
   {
      orc_Data.Append("");
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Store assembled data in file

   \param[in] orc_Data                 converted data to string list
   \param[in] orc_Path                 storage path for created file
   \param[in] orc_DataPoolName         name of data pool
   \param[in] oq_FileType              .c or .h file selected

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store file

   \created     24.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportDataPool::mh_SaveToFile(C_SCLStringList & orc_Data, const C_SCLString & orc_Path,
                                          const C_SCLString & orc_DataPoolName, const bool oq_FileType)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_PathAndFilename;
   C_SCLString c_FileName;

   // assemble filename
   mh_CreateFileName(orc_DataPoolName, c_FileName);

   // assemble path and filename
   // add path + add filename + extension
   c_PathAndFilename = orc_Path + "/" + c_FileName;

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      c_PathAndFilename += ".h";
   }
   else
   {
      c_PathAndFilename += ".c";
   }

   // store into file
   try
   {
      orc_Data.SaveToFile(c_PathAndFilename);
   }
   catch (...)
   {
      osc_write_log_error("Creating source code", "Could not write to file \"" + c_PathAndFilename + "\"");
      s32_Retval = C_RD_WR;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create filename

   \param[in]  orc_DataPoolName         name of data pool
   \param[out] orc_FileName             assembled filename

   \created     17.10.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
void C_OSCExportDataPool::mh_CreateFileName(const C_SCLString & orc_DataPoolName, C_SCLString & orc_FileName)
{
   // assemble filename
   // add data pool name + 'data_pool'
   orc_FileName = orc_DataPoolName.LowerCase() + "_data_pool";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get type string according to data type

   \param[in] oe_Type            data type (uint8, sint8, ...)

   \return
   data type as string

   \created     30.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_GetType(const C_OSCNodeDataPoolContent::E_Type oe_Type)
{
   C_SCLString c_Type;

   switch (oe_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8: // Data type unsigned 8 bit integer
      c_Type += "uint8";
      break;
   case C_OSCNodeDataPoolContent::eUINT16: // Data type unsigned 16 bit integer
      c_Type += "uint16";
      break;
   case C_OSCNodeDataPoolContent::eUINT32: // Data type unsigned 32 bit integer
      c_Type += "uint32";
      break;
   case C_OSCNodeDataPoolContent::eUINT64: // Data type unsigned 64 bit integer
      c_Type += "uint64";
      break;
   case C_OSCNodeDataPoolContent::eSINT8: // Data type signed 8 bit integer
      c_Type += "sint8";
      break;
   case C_OSCNodeDataPoolContent::eSINT16: // Data type signed 16 bit integer
      c_Type += "sint16";
      break;
   case C_OSCNodeDataPoolContent::eSINT32: // Data type signed 32 bit integer
      c_Type += "sint32";
      break;
   case C_OSCNodeDataPoolContent::eSINT64: // Data type signed 64 bit integer
      c_Type += "sint64";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32: // Data type 32 bit floating point
      c_Type += "float32";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64: // Data type 64 bit floating point
      c_Type += "float64";
      break;
   default:
      break;
   }

   return c_Type;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get type prefix according to data type

   \param[in] oe_Type                 data type (uint8, sint8, ...)
   \param[in] oq_IsArray              data type is an array

   \return
   data type as variable prefix string

   \created     28.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_GetTypePrefix(const C_OSCNodeDataPoolContent::E_Type oe_Type, const bool oq_IsArray)
{
   C_SCLString c_Prefix;

   if (oq_IsArray == true)
   {
      c_Prefix = "a";
   }
   else
   {
      c_Prefix = "";
   }

   switch (oe_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8: // Data type unsigned 8 bit integer
      c_Prefix += "u8";
      break;
   case C_OSCNodeDataPoolContent::eUINT16: // Data type unsigned 16 bit integer
      c_Prefix += "u16";
      break;
   case C_OSCNodeDataPoolContent::eUINT32: // Data type unsigned 32 bit integer
      c_Prefix += "u32";
      break;
   case C_OSCNodeDataPoolContent::eUINT64: // Data type unsigned 64 bit integer
      c_Prefix += "u64";
      break;
   case C_OSCNodeDataPoolContent::eSINT8: // Data type signed 8 bit integer
      c_Prefix += "s8";
      break;
   case C_OSCNodeDataPoolContent::eSINT16: // Data type signed 16 bit integer
      c_Prefix += "s16";
      break;
   case C_OSCNodeDataPoolContent::eSINT32: // Data type signed 32 bit integer
      c_Prefix += "s32";
      break;
   case C_OSCNodeDataPoolContent::eSINT64: // Data type signed 64 bit integer
      c_Prefix += "s64";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32: // Data type 32 bit floating point
      c_Prefix += "f32";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64: // Data type 64 bit floating point
      c_Prefix += "f64";
      break;
   default:
      break;
   }

   return c_Prefix;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get value string from list element

   \param[in] orc_Value                value to be converted
   \param[in] oe_Type                  type of element
   \param[in] oq_IsArray               indicator for array type

   \return
   element with unsigned indicator as string

   \created     29.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_GetElementValueString(const C_OSCNodeDataPoolContent & orc_Value,
                                                          const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                          const bool oq_IsArray)
{
   C_SCLString c_String;
   sint64 s64_Value;
   sint32 s32_Value;

   if (oq_IsArray == false)
   {
      switch (oe_Type)
      {
      case C_OSCNodeDataPoolContent::eUINT8: ///< Data type unsigned 8 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueU8()) + "U";
         break;
      case C_OSCNodeDataPoolContent::eUINT16: ///< Data type unsigned 16 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueU16()) + "U";
         break;
      case C_OSCNodeDataPoolContent::eUINT32: ///< Data type unsigned 32 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueU32()) + "UL";
         break;
      case C_OSCNodeDataPoolContent::eUINT64: ///< Data type unsigned 64 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueU64()) + "ULL";
         break;
      case C_OSCNodeDataPoolContent::eSINT8: ///< Data type signed 8 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueS8());
         break;
      case C_OSCNodeDataPoolContent::eSINT16: ///< Data type signed 16 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueS16());
         break;
      case C_OSCNodeDataPoolContent::eSINT32: ///< Data type signed 32 bit integer
         s32_Value = orc_Value.GetValueS32();
         if (s32_Value == std::numeric_limits<sint32>::min())
         {
            //workaround for GCC warning when reaching lowest possible value
            c_String += C_SCLString::IntToStr(std::numeric_limits<sint32>::min() + 1) + "L - 1";
         }
         else
         {
            c_String += C_SCLString::IntToStr(orc_Value.GetValueS32()) + "L";
         }
         break;
      case C_OSCNodeDataPoolContent::eSINT64: ///< Data type signed 64 bit integer
         s64_Value = orc_Value.GetValueS64();
         if (s64_Value == std::numeric_limits<sint64>::min())
         {
            //workaround for GCC warning when reaching lowest possible value
            c_String += C_SCLString::IntToStr(std::numeric_limits<sint64>::min() + 1) + "LL - 1";
         }
         else
         {
            c_String += C_SCLString::IntToStr(orc_Value.GetValueS64()) + "LL";
         }
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32: ///< Data type 32 bit floating point
         c_String += C_SCLString::FloatToStr(orc_Value.GetValueF32()) + "F";
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64: ///< Data type 64 bit floating point
         //depending on the value this could result in a very long string
         //but with other approaches (e.g. "printf formatter %g" we might lose precision
         c_String += C_SCLString::FloatToStr(orc_Value.GetValueF64());
         break;
      default:
         break;
      }
   }
   else
   {
      const uint32 u32_ArraySize = orc_Value.GetArraySize();
      c_String = "{ ";
      for (uint32 u32_ArrayIndex = 0U; u32_ArrayIndex < u32_ArraySize; u32_ArrayIndex++)
      {
         switch (oe_Type)
         {
         case C_OSCNodeDataPoolContent::eUINT8: ///< Data type unsigned 8 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAU8Element(u32_ArrayIndex)) + "U";
            break;
         case C_OSCNodeDataPoolContent::eUINT16: ///< Data type unsigned 16 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAU16Element(u32_ArrayIndex)) + "U";
            break;
         case C_OSCNodeDataPoolContent::eUINT32: ///< Data type unsigned 32 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAU32Element(u32_ArrayIndex)) + "UL";
            break;
         case C_OSCNodeDataPoolContent::eUINT64: ///< Data type unsigned 64 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAU64Element(u32_ArrayIndex)) + "ULL";
            break;
         case C_OSCNodeDataPoolContent::eSINT8: ///< Data type signed 8 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAS8Element(u32_ArrayIndex));
            break;
         case C_OSCNodeDataPoolContent::eSINT16: ///< Data type signed 16 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAS16Element(u32_ArrayIndex));
            break;
         case C_OSCNodeDataPoolContent::eSINT32: ///< Data type signed 32 bit integer
            s32_Value = orc_Value.GetValueAS32Element(u32_ArrayIndex);
            if (s32_Value == std::numeric_limits<sint32>::min())
            {
               //workaround for GCC warning when reaching lowest possible value
               c_String += C_SCLString::IntToStr(std::numeric_limits<sint32>::min() + 1) + "L - 1";
            }
            else
            {
               c_String += C_SCLString::IntToStr(orc_Value.GetValueAS32Element(u32_ArrayIndex)) + "L";
            }
            break;
         case C_OSCNodeDataPoolContent::eSINT64: ///< Data type signed 64 bit integer
            s64_Value = orc_Value.GetValueAS64Element(u32_ArrayIndex);
            if (s64_Value == std::numeric_limits<sint64>::min())
            {
               //workaround for GCC warning when reaching lowest possible value
               c_String += C_SCLString::IntToStr(std::numeric_limits<sint64>::min() + 1) + "LL - 1";
            }
            else
            {
               c_String += C_SCLString::IntToStr(orc_Value.GetValueAS64Element(u32_ArrayIndex)) + "LL";
            }
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32: ///< Data type 32 bit floating point
            c_String += C_SCLString::FloatToStr(orc_Value.GetValueAF32Element(u32_ArrayIndex)) + "F";
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64: ///< Data type 64 bit floating point
            c_String += C_SCLString::FloatToStr(orc_Value.GetValueAF64Element(u32_ArrayIndex));
            break;
         default:
            break;
         }

         if (u32_ArrayIndex == (u32_ArraySize - 1U))
         {
            c_String += " }";
         }
         else
         {
            c_String += ", ";
         }
      }
   }

   return c_String;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get element size according to data type

   \param[in] oe_Type                 data type (uint8, sint8, ...)
   \param[in] ou32_ArraySize          size of array (if ou8_DataType means an array)
   \param[in] oq_IsArray              data type is an array

   \return
   element size as string

   \created     30.08.2017  STW/U.Roesch
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_GetElementSize(const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                   const uint32 ou32_ArraySize, const bool oq_IsArray)
{
   C_SCLString c_Size;
   uint32 u32_Size = 0U;

   switch (oe_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8: // Data type unsigned 8 bit integer
      u32_Size = 1U;
      break;
   case C_OSCNodeDataPoolContent::eUINT16: // Data type unsigned 16 bit integer
      u32_Size = 2U;
      break;
   case C_OSCNodeDataPoolContent::eUINT32: // Data type unsigned 32 bit integer
      u32_Size = 4U;
      break;
   case C_OSCNodeDataPoolContent::eUINT64: // Data type unsigned 64 bit integer
      u32_Size = 8U;
      break;
   case C_OSCNodeDataPoolContent::eSINT8: // Data type signed 8 bit integer
      u32_Size = 1U;
      break;
   case C_OSCNodeDataPoolContent::eSINT16: // Data type signed 16 bit integer
      u32_Size = 2U;
      break;
   case C_OSCNodeDataPoolContent::eSINT32: // Data type signed 32 bit integer
      u32_Size = 4U;
      break;
   case C_OSCNodeDataPoolContent::eSINT64: // Data type signed 64 bit integer
      u32_Size = 8U;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32: // Data type 32 bit floating point
      u32_Size = 4U;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64: // Data type 64 bit floating point
      u32_Size = 8U;
      break;
   default:
      break;
   }

   if (oq_IsArray == true)
   {
      u32_Size *= ou32_ArraySize;
   }

   c_Size = C_SCLString::IntToStr(u32_Size);

   return c_Size;
}