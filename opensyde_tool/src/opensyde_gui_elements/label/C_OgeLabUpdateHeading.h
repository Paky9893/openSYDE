//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for update screen heading (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.02.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABUPDATEHEADING_H
#define C_OGELABUPDATEHEADING_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabUpdateHeading :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabUpdateHeading(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif