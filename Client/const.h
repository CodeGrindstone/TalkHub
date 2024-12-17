#ifndef CONST_H
#define CONST_H

#include <QLineEdit>
#include <QString>
#include <QStyle>
void setupPasswordToggle(QLineEdit *lineEdit, const QString &visibleIconPath, const QString &invisibleIconPath);

enum ClickLbState{
    Normal = 0,
    Selected = 1
};

extern std::function<void(QWidget*)> repolish;
#endif // CONST_H
