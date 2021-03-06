//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for identifying data temporarily  (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSTEMPORARYDATAID_H
#define C_PUIBSTEMPORARYDATAID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <exception>
#include "stwtypes.h"
#include "C_PuiSdDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsTemporaryDataID
{
public:
   C_PuiBsTemporaryDataID(const stw_types::sint32 & ors32_Type, const stw_types::uint32 & oru32_Index);

   bool operator <(const C_PuiBsTemporaryDataID & orc_Other) const;

   stw_types::sint32 s32_Type;
   stw_types::uint32 u32_Index;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
