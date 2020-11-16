
#pragma once

#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>

using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ProgressBarScreenController;

/**
 * Pop ups view
 * @author andreas
 */
class tdme::tools::shared::views::PopUps final
{
private:
	InfoDialogScreenController* infoDialogScreenController { nullptr };
	FileDialogScreenController* fileDialogScreenController { nullptr };
	ProgressBarScreenController* progressBarScreenController { nullptr };

public:
	/**
	 * Public constructor
	 */
	PopUps();

	/**
	 * Destructor
	 */
	 ~PopUps();

	/**
	 * @return file dialog screen controller
	 */
	 FileDialogScreenController* getFileDialogScreenController();

	/**
	 * @return info dialog screen controller
	 */
	 InfoDialogScreenController* getInfoDialogScreenController();

	/**
	 * @return progress bar screen controller
	 */
	 ProgressBarScreenController* getProgressBarScreenController();

	/**
	 * Init
	 */
	 void initialize();

	/**
	 * Dispose
	 */
	 void dispose();

};
