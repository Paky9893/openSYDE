//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for dashboard undo command stack (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDAUNOMANAGER_H
#define C_SYVDAUNOMANAGER_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include <QGraphicsScene>

#include "C_PuiSvDashboard.h"
#include "C_PuiSvDbDataElement.h"
#include "C_SebUnoBaseManager.h"
#include "C_PuiSvReadDataConfiguration.h"

#include "C_SebUtil.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaUnoManager :
   public C_SebUnoBaseManager
{
public:
   C_SyvDaUnoManager(QGraphicsScene * const opc_Scene, QObject * const opc_Parent = NULL);
   virtual ~C_SyvDaUnoManager(void);

   virtual void DoDelete(const QList<QGraphicsItem *> & orc_Items) override;
   void DoAddGeneric(const C_PuiSvDbDataElement::E_Type & ore_Type, const stw_types::uint64 & oru64_UniqueID,
                     const QPointF & orc_NewPos, const bool & orq_DarkModeDefault,
                     const QString & orc_AdditionalInformation = "");
   void DoAddSnapshot(const std::vector<stw_types::uint64> & oru64_UniqueIDs, const C_PuiSvDashboard & orc_Snapshot,
                      const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                 stw_opensyde_gui_logic::C_PuiSvReadDataConfiguration> & orc_RestoredRails,
                      const QPointF & orc_NewPos);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif