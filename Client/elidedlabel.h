#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QLabel>
#include <QFontMetrics>
#include <QResizeEvent>

class ElidedLabel : public QLabel {
    Q_OBJECT

public:
    explicit ElidedLabel(const QString &text, QWidget *parent = nullptr)
        : QLabel(parent), originalText(text) {
        setTextInteractionFlags(Qt::NoTextInteraction); // 禁止文本交互
        setWordWrap(false); // 禁止换行
        setAlignment(Qt::AlignLeft | Qt::AlignVCenter); // 左对齐
        updateElidedText(); // 初次设置省略文本
    }

    void setOriginalText(const QString &text) {
        originalText = text;
        updateElidedText(); // 更新省略文本
    }

    QString getOriginalText() const {
        return originalText;
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        QLabel::resizeEvent(event);
        updateElidedText(); // 窗口调整时更新省略文本
    }

private:
    QString originalText;

    void updateElidedText() {
        QFontMetrics metrics(font());
        int availableWidth = width(); // 获取当前控件宽度
        if (availableWidth > 0) {
            QString elided = metrics.elidedText(originalText, Qt::ElideRight, availableWidth);
            QLabel::setText(elided);
        }
    }
};

#endif // ELIDEDLABEL_H
