/* ********************************************************
   *  dirDialog.cpp
   *
   *  Created by Geoff R. Mclane, Paris
   *  (C) Dec 2011 GPL2 (or later)
   *
   *  A rather crude getFileName capability
   *
   ******************************************************** */
#include "dirDialog.h"
#include "utilities/utilities.h"

dirDialog::dirDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Select Directory");
    setMinimumSize(550,450);
    got_cancel = false;
    optionFlag = 0;

    int m = 10;
    int row = 0;
    int cols = 8;   // better if EVEN - see cols/2 below

    mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(m,m,m,m);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);

    info1 = new QLabel("Navigate via the tree list below, or enter exising directory Name: and Path:.",this);
    mainLayout->addWidget(info1,row,0,1,cols);
    row++;

    // ***TBD*** Need to understand how to force the Name: and Path:
    // labels to their smallest. Maybe use the 'friend' idea, or
    // set maximum size, or something...
    // file name row
    nameLab = new QLabel("Name:",this);
    nameLab->setAlignment(Qt::AlignRight);
    nameEd = new QLineEdit(this);
    // add the Name:
    mainLayout->addWidget(nameLab,row,0,1,1);
    mainLayout->addWidget(nameEd,row,1,1,cols-1);
    // add change handler
    connect(nameEd,SIGNAL(textChanged(QString)),this,SLOT(on_name_change(QString)));
    row++;  // bump ROW

    // path row
    pathLab = new QLabel("Path:",this);
    pathLab->setAlignment(Qt::AlignRight);
    pathEd = new QLineEdit(this);
    // add the Path:
    mainLayout->addWidget(pathLab,row,0,1,1);
    mainLayout->addWidget(pathEd,row,1,1,cols-1);
    // add change handler
    connect(pathEd,SIGNAL(textChanged(QString)),this,SLOT(on_path_change(QString)));
    row++;  // bump ROW

    info2 = new QLabel("Above RED if does not exist. Green if ok. Below click to select, double-click to browse.",this);
    mainLayout->addWidget(info2,row,0,1,cols);
    row++;

    //= Directory Tree
    treeWidget = new QTreeWidget(this);
    treeWidget->setAlternatingRowColors(true);
    treeWidget->setRootIsDecorated(true);
    treeWidget->setUniformRowHeights(true);
    treeWidget->setSortingEnabled(false);
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    // ***TBD*** How to make the 'Type' column smaller
    // ***TBD*** Maybe make the 'Type' column hidden, and add a visible ICON
    // Need ICONS - Directory, File (and Drives for windows)
    treeWidget->headerItem()->setText(D_TYPE, "Type");
    treeWidget->headerItem()->setText(D_NAME, "Name");

    connect( treeWidget,
             SIGNAL( itemSelectionChanged() ),
             this,
             SLOT( on_tree_selection_changed() )
    );

    connect( treeWidget,
             SIGNAL( doubleClicked(QModelIndex) ),
             this,
             SLOT( on_tree_double_clicked(QModelIndex)) );

    mainLayout->addWidget(treeWidget,row,0,10,cols);
    row += 10;

    cancelButton = new QPushButton(this);
    cancelButton->setText("Cancel");
    mainLayout->addWidget(cancelButton,row,0,1,(int)(cols/2));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(on_cancel()));

    okButton = new QPushButton(this);
    okButton->setText("Select");
    okButton->setAutoDefault(true);
    mainLayout->addWidget(okButton,row,(int)(cols/2),1,(int)(cols/2));
    connect(okButton, SIGNAL(clicked()), this, SLOT(on_ok()));

    row++;

}

// ***TBD*** Need to separate into directory + last-directory
// here done very manually, but maybe there are Qt services to
// do this path parsing
void dirDialog::init(QString title, QString prev)
{
    if (title.length())
        setWindowTitle(title);
    int ind, siz;
    QString tmp = QDir::cleanPath(prev);    // seems this REMOVES a trailing '/'!
    util_ensureUnixPathSep(tmp);
    siz = tmp.length();
    if (siz) {
        ind = tmp.lastIndexOf(QChar('/'));
        if (ind == (siz-1)) {
            tmp.chop(1);    // remove trailing item
        }
    } else {
        tmp = util_getCurrentWorkDirectory();
    }
    QString fil = tmp;
    siz = tmp.length();
    ind = tmp.lastIndexOf(QChar('/'));
    // march to last '/' in path
    if (ind > 0) {
        fil = tmp.right(siz - ind - 1); // get last, aftet last '/'
        tmp = tmp.left(ind);    // get directory, before last '/'
    } else if (ind == 0) {
        fil = tmp.right(siz - 1);
        tmp = ST_ROOT;
    }
    nameEd->setText(fil);   // last becomes name
    pathEd->setText(tmp);   // before becomes path
    fill_tree();            // fill the TREE/LIST
    on_name_change(fil);
    on_path_change(tmp);
}


// this ONLY applies to WINDOWS, but the service apparently exists in
// all platforms, but is ignored, since there are no logical drives
int dirDialog::getDriveCount()
{
    int i;
    int cnt = 0;
    QFileInfoList drvList = QDir::drives();
    for (i = 0; i < drvList.count(); i++) {
        QFileInfo fi = drvList.at(i);
        QDir d(fi.filePath());
        if (!d.exists() || !d.isReadable())
            continue;   // do NOT add NON existant DRIVES
        cnt++;
    }
    return cnt;
}

void dirDialog::fill_tree()
{
    int i;
    QString tmp(pathEd->text());    // get the PATH
    // fill the tree
#ifdef Q_OS_WIN
    if (tmp == ST_DRIVES) {
        QFileInfoList drvList = QDir::drives();
        // int cnt = getDriveCount();
        treeWidget->setUpdatesEnabled(false);
        treeWidget->model()->removeRows(0, treeWidget->model()->rowCount());
        QTreeWidgetItem *parentItem; // = new QTreeWidgetItem();
        parentItem = treeWidget->invisibleRootItem();
        for (i = 0; i < drvList.count(); i++) {
            QFileInfo fi = drvList.at(i);
            QDir d(fi.filePath());
            if (!d.exists() || !d.isReadable())
                continue;   // do NOT add NON existant DRIVES
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(parentItem);
            fileItem->setText(D_TYPE,"Dr");
            fileItem->setText(D_NAME,fi.filePath());
        }
        treeWidget->setUpdatesEnabled(true);
        return;
    }
#endif
    if (tmp == ST_ROOT)
        tmp = "/";
    QStringList dList = findDirs(tmp,false);        // get directory list

    treeWidget->setUpdatesEnabled(false);
    treeWidget->model()->removeRows(0, treeWidget->model()->rowCount());
    QTreeWidgetItem *parentItem; // = new QTreeWidgetItem();
    parentItem = treeWidget->invisibleRootItem();
    QTreeWidgetItem *item;
    if (tmp != "/") {
        item = new QTreeWidgetItem(parentItem);
        item->setText(D_TYPE,"D");
        item->setText(D_NAME,"..");
    }
    int ind;
    QString fil;
    for (i = 0; i < dList.size(); ++i) {
        fil = dList.at(i);
        ind = fil.lastIndexOf(QChar('/'));
        // march to last '/' in path
        if (ind >= 0) {
            fil = fil.mid(ind + 1); // get only LAST
        }
        item = new QTreeWidgetItem(parentItem);
        item->setText(D_TYPE,"D");
        item->setText(D_NAME,fil);
    }
    treeWidget->setUpdatesEnabled(true);
}

void dirDialog::on_tree_selection_changed()
{
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) {
        return;
    }
    QString type = item->text(D_TYPE);
    QString name = item->text(D_NAME);
    //= Check there is item and it has a name
    if ((type.length() == 0)||(name.length() == 0)) {
        return;
    }
    if (name != "..") {
        nameEd->setText(name);
        on_name_change(name);
    }
}

void dirDialog::on_tree_double_clicked(QModelIndex mi)
{
    Q_UNUSED(mi);
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) {
        return;
    }
    QString type = item->text(D_TYPE);
    QString name = item->text(D_NAME);
    //= Check there is item and it has a name
    if ((type.length() == 0)||(name.length() == 0)) {
        outLog("on_tree_selection_changed: no D_TYPE ir D_NAME item");
        nameEd->setText("");
        return;
    }
    QString path = pathEd->text();
    int ind, siz;
    QString tmp;
    if (type == "D") {
        if (name == "..") {
            // go up one level if possible
            if (path == ST_ROOT)
                return;
            tmp = path;
            siz = 0;
            ind = tmp.indexOf(QChar('/'));
            // march to last '/' in path
            while (ind >= 0) {
                tmp = tmp.mid(ind + 1);
                siz = tmp.length();
                ind = tmp.indexOf(QChar('/'));
            }
            if (siz) {
                path.chop(siz+1);   // remove last entry, and separator
                if (path.length() == 0)
                    path = ST_ROOT;
                pathEd->setText(path);
            }
#ifdef Q_OS_WIN
            else {
                // up from 'C:' is a list of active DRIVES
                //QFileInfoList dList = QDir::drives();
                int cnt = getDriveCount();
                if (cnt > 1) {
                    path = ST_DRIVES;
                    pathEd->setText(path);
                }
                // ***TBD*** Perhaps some WARNING that we are on the ONLY
                // valid readable DRIVE, and maybe should NOT add a '..'
                // token in this case
            }
#endif
        } else {
            // go down into this directory
            if (path == ST_ROOT) {
                path= "";
            }
            path.append("/"+name);
            pathEd->setText(path);
        }
        fill_tree(); // and reset the LIST
        on_path_change(path);
#ifdef Q_OS_WIN
    } else if (type == "Dr") {
        // have selected a DRIVE
        if ((name.length() > 2)&&(name.at(2) == QChar('/'))) {
            name.chop(1);   // drop the trailing
        }
        pathEd->setText(name);  // set this DRIVE
        fill_tree();
        on_path_change(name);
#endif
    }
}

void dirDialog::on_name_change(QString in)
{
    QString style("color:red");
    QString txt(in);
    QString path(pathEd->text());
    if (path == ST_ROOT)
        path = "";
    QDir dir(path+"/"+txt);
    bool disabled = true;
    if (dir.exists()) {
        style = "color:green";
        disabled = false;
    }
    okButton->setDisabled(disabled);
    nameLab->setStyleSheet(style);
}

void dirDialog::on_path_change(QString in)
{
    // FORCE the separator ALWAYS to '/', since manual user input allowed
    QString txt(in);
    if (util_ensureUnixPathSep(txt))
        pathEd->setText(txt);   // put back correct string
    QDir dir(txt);
    QString style("color:red");
    if (dir.exists() || (txt == ST_ROOT)
#ifdef Q_OS_WIN
        || (txt == ST_DRIVES)
#endif
        ) {
        style = "color:green";
        on_name_change(nameEd->text());
        fill_tree();
    }
    pathLab->setStyleSheet(style);
}

void dirDialog::on_ok()
{
    close();
}

void dirDialog::on_cancel()
{
    nameEd->setText("");
    got_cancel = true;
    close();
}

// pass back the RESULTS
QString dirDialog::getDirName()
{
    if (got_cancel)
        return "";  // user cancelled, to info NOT valid
    QString name = nameEd->text();
    QString path = pathEd->text();
    if (name.length() == 0)
        return name;
    if (path == ST_ROOT)
        return "/"+name;
    return path+"/"+name;
}

// eof - dirDialog.cpp
