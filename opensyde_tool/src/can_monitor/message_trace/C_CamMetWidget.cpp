//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for showing entire CAN trace and its buttons (implementation)

   Widget for showing entire CAN trace and its buttons

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamMetWidget.h"
#include "ui_C_CamMetWidget.h"

#include "stwerrors.h"
#include "C_Uti.h"
#include "C_CamOgeWiSectionHeader.h"
#include "C_GtGetText.h"
#include "C_OSCSystemBus.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     15.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_CamMetWidget::C_CamMetWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMetWidget),
   mq_OsySysDef(false)
{
   this->mpc_Ui->setupUi(this);

   this->SetBackgroundColor(11);

   this->mpc_Ui->pc_WiTitle->SetTitle(C_GtGetText::h_GetText("Trace"));
   this->mpc_Ui->pc_WiTitle->SetIconType(C_CamOgeWiSectionHeader::E_ButtonType::eNOBUTTON);

   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigStartLogging,
           this, &C_CamMetWidget::m_StartLogging);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigPauseLogging,
           this, &C_CamMetWidget::SigPauseLogging);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigStopLogging,
           this, &C_CamMetWidget::SigStopLogging);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigContinueLogging,
           this, &C_CamMetWidget::SigContinueLogging);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigClearData, this, &C_CamMetWidget::ClearData);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigDisplayAsHex, this->mpc_Ui->pc_TraceView,
           &C_CamMetTreeView::SetDisplayAsHex);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigDisplayTimestampRelative,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetDisplayTimestampRelative);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigChangeProtocol,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetProtocol);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigDisplayTree,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetDisplayTree);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigDisplayUniqueMessages,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetDisplayUniqueMessages);

   // File loading
   connect(&this->mc_DatabaseTimer, &QTimer::timeout, this, &C_CamMetWidget::m_DatabaseTimer);
   this->mc_DatabaseTimer.setInterval(100);

   // Status bar update
   connect(&this->mc_StatusBarTimer, &QTimer::timeout, this, &C_CamMetWidget::m_StatusBarTimer);
   this->mc_StatusBarTimer.start(500);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     15.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_CamMetWidget::~C_CamMetWidget()
{
   delete this->mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load all user settings

   \created     22.11.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::LoadUserSettings(void)
{
   this->mpc_Ui->pc_ControlWidget->LoadUserSettings();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save all user settings

   \created     28.09.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::SaveUserSettings(void) const
{
   this->mpc_Ui->pc_ControlWidget->SaveUserSettings();
   this->mpc_Ui->pc_TraceView->SaveUserSettings();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Stopping logging in case of an error

   \created     30.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::StopLogging(void)
{
   this->mpc_Ui->pc_ControlWidget->StopLogging();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clears the previous communication

   \created     23.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::ClearData(void)
{
   this->mpc_Ui->pc_TraceView->ActionClearData();
   this->mpc_Ui->pc_StatusWidget->SetBusLoad(0U);
   this->mpc_Ui->pc_StatusWidget->SetTxErrors(0U);
   this->mpc_Ui->pc_StatusWidget->SetFilteredMessages(0U);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds new filter configurations

   \param[in]  orc_FilterItems     Filter configurations to add

   \return
   possible return value(s) and description

   \created     14.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::AddFilterItems(const QList<C_CamProFilterItemData> & orc_FilterItems)
{
   QList<C_CamProFilterItemData>::const_iterator c_ItItem;

   for (c_ItItem = orc_FilterItems.begin(); c_ItItem != orc_FilterItems.end(); ++c_ItItem)
   {
      this->mpc_Ui->pc_TraceView->AddFilter(*c_ItItem);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes specific filter configurations

   \param[in]     orc_FilterItems         All filter configurations for removing

   \return
   possible return value(s) and description

   \created     14.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::RemoveFilterItems(const QList<C_CamProFilterItemData> & orc_FilterItems)
{
   QList<C_CamProFilterItemData>::const_iterator c_ItItem;

   for (c_ItItem = orc_FilterItems.begin(); c_ItItem != orc_FilterItems.end(); ++c_ItItem)
   {
      this->mpc_Ui->pc_TraceView->RemoveFilter(*c_ItItem);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes the entire filter configuration

   \created     14.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::RemoveAllFilters(void)
{
   this->mpc_Ui->pc_TraceView->RemoveAllFilter();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the status bar filter label

   \param[in]     ou32_ActiveFilters         Count of active filters

   \created     29.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::SetActiveFilters(const stw_types::uint32 ou32_ActiveFilters) const
{
   this->mpc_Ui->pc_StatusWidget->SetActiveFilters(ou32_ActiveFilters);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a new openSYDE system definition

   \param[in]     orc_PathSystemDefinition       Path of system definition file (Must be .syde_sysdef)
   \param[in]     os32_BusIndex                  Bus index of CAN bus of system definition for monitoring
                                                 Set to -1 if no known yet

   \created     19.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::AddDatabaseOsySysDef(const QString & orc_PathSystemDefinition, const sint32 os32_BusIndex)
{
   sint32 s32_Return;

   if (os32_BusIndex >= 0)
   {
      s32_Return = this->mpc_Ui->pc_TraceView->StartAddOsySysDef(orc_PathSystemDefinition.toStdString().c_str(),
                                                                 static_cast<uint32>(os32_BusIndex));
   }
   else
   {
      s32_Return = this->mpc_Ui->pc_TraceView->StartAddOsySysDef(orc_PathSystemDefinition.toStdString().c_str());
   }

   if (s32_Return == C_NO_ERR)
   {
      this->mq_OsySysDef = true;
      this->mc_DatabaseTimer.start(100);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a new DBC file

   \param[in]     orc_PathSystemDefinition       Path of system definition file (Must be .dbc)

   \created     19.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::AddDatabaseDbc(const QString & orc_PathDbc)
{
   const sint32 s32_Return = this->mpc_Ui->pc_TraceView->StartAddDbcFile(orc_PathDbc.toStdString().c_str());

   if (s32_Return == C_NO_ERR)
   {
      this->mq_OsySysDef = false;
      this->mc_DatabaseTimer.start(100);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes a database for interpretation

   \param[in]     orc_PathDatabase         Path with file name of database

   \created     19.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::RemoveDatabase(const QString & orc_PathDatabase)
{
   this->mpc_Ui->pc_TraceView->RemoveDatabase(orc_PathDatabase.toStdString().c_str());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activates a database for interpretation

   \param[in]     orc_PathDatabase         Path with file name of database
   \param[in]     oq_Active                Flag if database shall be active or not for interpretation

   \created     19.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::ActivateDatabase(const QString & orc_PathDatabase, const bool oq_Active)
{
   this->mpc_Ui->pc_TraceView->ActivateDatabase(orc_PathDatabase.toStdString().c_str(), oq_Active);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the set bus index of the specific system definition

   This function is thread safe.

   The result of the function will be sent by signal SigDatabaseSetOsySysDefBusResult
   C_NO_ERR    Bus index for this system definition adapted
   C_NOACT     No system definition found with this path

   \param[in]     orc_PathSystemDefinition       Path of system definition file (Must be .syde_sysdef)
   \param[in]     ou32_BusIndex                  Bus index of CAN bus of system definition for monitoring

   \created     19.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::SetDatabaseOsySysDefBus(const QString & orc_PathSystemDefinition,
                                             const stw_types::uint32 ou32_BusIndex)
{
   sint32 s32_Return;

   s32_Return = this->mpc_Ui->pc_TraceView->SetOsySysDefBus(orc_PathSystemDefinition.toStdString().c_str(),
                                                            ou32_BusIndex);
   Q_EMIT (this->SigDatabaseSetOsySysDefBusResult(orc_PathSystemDefinition, s32_Return));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new ASC file logging.

   \param[in]     orc_FilePath     file path to log file

   \created     17.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::AddLogFileAsc(const QString & orc_FilePath)
{
   const sint32 s32_Result =
      this->mpc_Ui->pc_TraceView->AddLogFileAsc(orc_FilePath.toStdString().c_str(),
                                                this->mpc_Ui->pc_TraceView->GetDisplayAsHex(),
                                                this->mpc_Ui->pc_TraceView->GetDisplayTimestampRelative());

   Q_EMIT (this->SigLogFileAddResult(s32_Result));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new BLF file logging.

   \param[in]     orc_FilePath     file path to log file

   \created     17.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::AddLogFileBlf(const QString & orc_FilePath)
{
   const sint32 s32_Result = this->mpc_Ui->pc_TraceView->AddLogFileBlf(orc_FilePath.toStdString().c_str());

   Q_EMIT (this->SigLogFileAddResult(s32_Result));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove all configured log files.

   \created     17.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::RemoveAllLogFiles(void) const
{
   this->mpc_Ui->pc_TraceView->RemoveAllLogFiles();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the used message monitor

   \return
   Pointer to message logger

   \created     15.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
stw_opensyde_gui_logic::C_SyvComMessageMonitor * C_CamMetWidget::GetMessageMonitor(void) const
{
   return this->mpc_Ui->pc_TraceView;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Stops the logging

   \created     25.01.2019  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::m_StartLogging(void)
{
   this->mpc_Ui->pc_TraceView->ActionClearData();

   Q_EMIT (this->SigStartLogging());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Timer function for updating status bar

   \created     16.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::m_StatusBarTimer(void)
{
   this->mpc_Ui->pc_StatusWidget->SetBusLoad(this->mpc_Ui->pc_TraceView->GetBusLoad());
   this->mpc_Ui->pc_StatusWidget->SetTxErrors(this->mpc_Ui->pc_TraceView->GetTxErrors());
   this->mpc_Ui->pc_StatusWidget->SetFilteredMessages(this->mpc_Ui->pc_TraceView->GetFilteredMessages());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks the progress of the current loading database file

   Starts and checks the thread for loading openSYDE system definition and DBC files

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMetWidget::m_DatabaseTimer(void)
{
   sint32 s32_ThreadResult;
   sint32 s32_Return;

   s32_Return = this->mpc_Ui->pc_TraceView->GetResults(s32_ThreadResult);

   if (s32_Return == C_NO_ERR)
   {
      // Nothing to do anymore
      this->mc_DatabaseTimer.stop();

      if (this->mq_OsySysDef == false)
      {
         // DBC file
         Q_EMIT (this->SigDatabaseLoadResultDbc(s32_ThreadResult));
      }
      else
      {
         std::vector<stw_opensyde_core::C_OSCSystemBus> c_Busses;
         this->mpc_Ui->pc_TraceView->GetResultBusses(c_Busses);

         // openSYDE system definition
         Q_EMIT (this->SigDatabaseLoadResultOsySysDef(s32_ThreadResult, c_Busses));
      }
   }
}