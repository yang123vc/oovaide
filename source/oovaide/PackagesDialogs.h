/*
 * PackagesDialogs.h
 *
 *  Created on: Jan 20, 2014
 *  \copyright 2014 DCBlaha.  Distributed under the GPL.
 */

#ifndef PACKAGESDIALOGS_H_
#define PACKAGESDIALOGS_H_

#include "Gui.h"
#include "Packages.h"

/// This dialog lets the user select system packages to add to the Oovaide project.
/// On Linux, this uses pkg-config to discover the system packages. On Windows,
/// a package file is read.
class AddPackageDialog:public Dialog
    {
    public:
        AddPackageDialog();
        ~AddPackageDialog();
        Package getPackage() const;
        // This is for external callbacks
        void selectPackage();

    private:
        GuiList mAllPackagesList;
        AvailablePackages mAvailPackages;
    };

/// This lists the packages in the Oovaide project, and select packages to view
/// or change details.
class ProjectPackagesDialog:public Dialog
    {
    public:
        ProjectPackagesDialog(std::string &baseBuildArgs);
        virtual ~ProjectPackagesDialog();

        // These are for external callbacks
        void selectPackage();
        void displayAddPackageDialog();
        void removePackage();
#ifndef __linux__
        void winScanDirectories();
        void winSetEnableScanning();
#endif

    private:
        GuiList mProjectPackagesList;
        ProjectPackages mProjectPackages;
        std::string &mBaseBuildArgs;
        std::string mLastSelectedPackage;
        bool mAllowSelection;
        virtual void beforeRun() override;
        virtual bool afterRun(bool ok) override;
        bool savePackage(const std::string &pkgName);
        void updatePackageList();
        void clearPackageDisplay();
    };

#endif /* PACKAGESDIALOGS_H_ */
