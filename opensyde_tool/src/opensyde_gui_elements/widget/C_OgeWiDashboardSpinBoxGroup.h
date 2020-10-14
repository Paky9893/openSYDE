//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for dashboard spin boxes of all available types (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIDASHBOARDSPINBOXGROUP_H
#define C_OGEWIDASHBOARDSPINBOXGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbSpinBox.h"
#include "C_OgeWiSpinBoxGroup.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiDashboardSpinBoxGroup :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   C_OgeWiDashboardSpinBoxGroup(QWidget * const opc_Parent = NULL);

   void AdjustFontToSize(void);

   void SetUnit(const QString & orc_Value);
   void SetShowUnit(const bool oq_ShowUnit);
   void SetDesignType(const stw_opensyde_gui_logic::C_PuiSvDbSpinBox::E_Type oe_Type);

protected:
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   QString mc_Unit;
   bool mq_ShowUnit;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
