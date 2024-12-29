#include "searchboxedit.h"


SearchBoxEdit::SearchBoxEdit(QWidget *parent):QLineEdit (parent),_max_len(0)
{
    connect(this, &QLineEdit::textChanged, this, &SearchBoxEdit::limitTextLength);
}
void SearchBoxEdit::SetMaxLength(int maxLen)
{
    _max_len = maxLen;
}
