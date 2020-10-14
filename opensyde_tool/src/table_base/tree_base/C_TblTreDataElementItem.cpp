//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data element tree item

   Data element tree item

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_TblTreDataElementItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_TblTreDataElementItem::mhc_Additional64BitInfo = " (Not supported, 64 bit value)";
const QString C_TblTreDataElementItem::mhc_AdditionalArrayInfo = " (Not supported, array or string type)";
const QString C_TblTreDataElementItem::mhc_AdditionalWriteOnlyInfo = " (Not supported, read only)";
const QString C_TblTreDataElementItem::mhc_AdditionalArrayIndexInfo = " (Not supported, array index)";
const QString C_TblTreDataElementItem::mhc_AdditionalArrayStringInfo = " (Not supported, string type)";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in]  oq_IsArrayItem    Flag for if item is an array
   \param[in]  oq_IsArray        Is array
   \param[in]  orc_OriginalName  Original name
   \param[in]  oe_Type           Type
   \param[in]  oe_Access         Access
   \param[in]  oq_IsString       Is string
   \param[in]  orc_Id            Id
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreDataElementItem::C_TblTreDataElementItem(const bool oq_IsArrayItem, const bool oq_IsArray,
                                                 const QString & orc_OriginalName,
                                                 const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                 const C_OSCNodeDataPoolListElement::E_Access oe_Access,
                                                 const bool oq_IsString,
                                                 const C_PuiSvDbNodeDataPoolListElementId & orc_Id) :
   C_TblTreItem(),
   mq_IsArrayItem(oq_IsArrayItem),
   mq_IsArray(oq_IsArray),
   mc_OriginalName(orc_OriginalName),
   me_Type(oe_Type),
   me_Access(oe_Access),
   mq_IsString(oq_IsString),
   mc_Id(orc_Id)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Configure dynamic name

   \param[in]  oq_ShowOnlyWriteElements   Optional flag to show only writable elements
   \param[in]  oq_ShowArrayElements       Optional flag to hide all array elements (if false)
   \param[in]  oq_ShowArrayIndexElements  Optional flag to hide all array index elements (if false)
   \param[in]  oq_Show64BitValues         Optional flag to hide all 64 bit elements (if false)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementItem::ConfigureDynamicName(const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                                   const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues)
{
   // Set the name always here, in case of an earlier update, the name was adapted with the explanation
   this->c_Name = this->mc_OriginalName;
   if (((this->mq_IsArrayItem) && (oq_ShowArrayIndexElements == true)) ||
       ((this->mq_IsArrayItem == false) && ((this->mq_IsArray == false) || (oq_ShowArrayElements == true))))
   {
      if ((((this->me_Type != C_OSCNodeDataPoolContent::eFLOAT64) &&
            (this->me_Type != C_OSCNodeDataPoolContent::eUINT64)) &&
           (this->me_Type != C_OSCNodeDataPoolContent::eSINT64)) ||
          (oq_Show64BitValues == true))
      {
         if ((this->me_Access == C_OSCNodeDataPoolListElement::eACCESS_RW) ||
             (oq_ShowOnlyWriteElements == false))
         {
            //Valid
            this->q_Enabled = true;
            this->q_Selectable = true;
         }
         else
         {
            this->q_Enabled = false;
            this->q_Selectable = false;
            //Explanation
            this->c_Name += C_TblTreDataElementItem::mhc_AdditionalWriteOnlyInfo;
         }
      }
      else
      {
         this->q_Enabled = false;
         this->q_Selectable = false;
         //Explanation
         this->c_Name += C_TblTreDataElementItem::mhc_Additional64BitInfo;
      }
   }
   else
   {
      if (this->mq_IsArrayItem)
      {
         this->q_Enabled = false;
         this->q_Selectable = false;
         //Explanation
         this->c_Name += C_TblTreDataElementItem::mhc_AdditionalArrayIndexInfo;
      }
      else
      {
         if (this->mq_IsString)
         {
            this->q_Enabled = false;
            this->q_Selectable = false;
            //Explanation
            this->c_Name += C_TblTreDataElementItem::mhc_AdditionalArrayStringInfo;
         }
         else
         {
            //If there are select-able child items don't disable the parent node
            if (oq_ShowArrayIndexElements)
            {
               this->q_Enabled = true;
            }
            else
            {
               this->q_Enabled = false;
            }
            this->q_Selectable = false;
            //If there are select-able child items don't add explanation
            if (oq_ShowArrayIndexElements == false)
            {
               //Explanation
               this->c_Name += C_TblTreDataElementItem::mhc_AdditionalArrayInfo;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get id

   \return
   Id
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbNodeDataPoolListElementId & C_TblTreDataElementItem::GetId(void) const
{
   return this->mc_Id;
}
