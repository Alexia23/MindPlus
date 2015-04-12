#include "CompleteLineEdit.h"
#include <QKeyEvent>
#include <QListView>
#include <QStringListModel>
#include <QDebug>
#include <QMap>
#include "DataStruct.h"

CompleteLineEdit::CompleteLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    initData();
}

void CompleteLineEdit::setCompleteList(const QStringList &listComplete)
{
    this->listComplete_ = listComplete;
}

void CompleteLineEdit::initData()
{
    pListView_ = new QListView(this);
	pListView_->setIconSize(QSize(40, 40));
//	pListView_->sets
    connect(pListView_, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotCompleteText(const QModelIndex &)));
    pListView_->setWindowFlags(Qt::ToolTip);	//鐢≒opup鐨勬椂鍊欎細闃诲
    pStandardItemModel_ = new QStandardItemModel(this);
    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(slotCompleterInputText(const QString &)));
}

void CompleteLineEdit::keyPressEvent(QKeyEvent *e)
{
    //
    if (!pListView_->isHidden())
    {
        int count = pListView_->model()->rowCount();	//涓€鍏辨湁澶氬皯椤?        //qDebug() << trUtf8("涓€鍏辨湁") << count;
        QModelIndex currentIndex = pListView_->currentIndex();

        switch(e->key())
        {
            case Qt::Key_Down:
            {// 鎸夊悜涓嬫柟鍚戦敭鏃讹紝绉诲姩鍏夋爣閫変腑 涓嬩竴涓?瀹屾垚鍒楄〃涓殑椤?                ////qDebug()<<QString::fromUtf8("鎸変笅鏂瑰悜閿笅");
                int row = currentIndex.row() + 1;
                if (row >= count)
                {
                    row = 0;
                }
                QModelIndex index = pListView_->model()->index(row, 0);
                pListView_->setCurrentIndex(index);
                break;
            }
            case Qt::Key_Up:
            {// 鎸夊悜涓嬫柟鍚戦敭鏃讹紝绉诲姩鍏夋爣閫変腑 涓婁竴涓?瀹屾垚鍒楄〃涓殑椤?                ////qDebug()<<QString::fromUtf8("鎸変笅鏂瑰悜閿笂");
                int row = currentIndex.row() - 1;
                if (row < 0)
                {
                    row = count - 1;
                }
                QModelIndex index = pListView_->model()->index(row, 0);
                pListView_->setCurrentIndex(index);
                break;
            }
            case Qt::Key_Escape:
            {// 鎸変笅Esc閿椂锛岄殣钘忓畬鎴愬垪琛?                pListView_->hide();
                emit signalEsc();
                break;
            }
            case Qt::Key_Return:
            {// 鎸変笅鍥炶溅閿椂锛屼娇鐢ㄥ畬鎴愬垪琛ㄤ腑閫変腑鐨勯」锛屽苟闅愯棌瀹屾垚鍒楄〃
                if (currentIndex.isValid())
                {
                    QString name = pListView_->currentIndex().data().toString();
                    setText(name);
                    emit signalSelectItem(name);
                }
                pListView_->hide();
                //qDebug()<<QString::fromUtf8("鍦ㄨ繖閲岃皟鍑烘祦浜戠殑瀵硅瘽妗唦~");
                break;
            }
            default:
            {// 鍏朵粬鎯呭喌锛岄殣钘忓畬鎴愬垪琛紝骞朵娇鐢≦LineEdit鐨勯敭鐩樻寜涓嬩簨浠?                pListView_->hide();
                QLineEdit::keyPressEvent(e);
                break;
            }
        }//end of switch
    }
    else
    {//鍡妸鍏朵粬鐨勪笢瑗挎墧缁欏熀绫绘€绘槸鏄庣煡鐨勯€夋嫨
        QLineEdit::keyPressEvent(e);
    }
}
void CompleteLineEdit::slotCompleterInputText(const QString &inputText)
{
    if (inputText.isEmpty())
    {
        pListView_->hide();
        return;
    }
    if ((inputText.length() > 1) && (!pListView_->isHidden()))
    {
        return;
    }

    // 濡傛灉瀹屾暣鐨勫畬鎴愬垪琛ㄤ腑鐨勬煇涓崟璇嶅寘鍚緭鍏ョ殑鏂囨湰锛屽垯鍔犲叆瑕佹樉绀虹殑瀹屾垚鍒楄〃涓蹭腑
    QStringList sl;

    QMap<QString, QString>::iterator iter = map_moduleName_moduleTag_.begin();
    for(; iter != map_moduleName_moduleTag_.end(); ++iter)
    {
		QString moduleName = iter.key();
        QString moduleTag = iter.value();
		if(moduleName.contains(inputText, Qt::CaseInsensitive))
        {
            sl << moduleName;
        }
		else if(moduleTag.contains(inputText, Qt::CaseInsensitive))
        {
            sl << moduleName;
        }
    }

    pStandardItemModel_->clear();

    foreach (QString word, sl)
    {
        QStandardItem *item = new QStandardItem(word);
		//item->setSizeHint(QSize(20, 200));
        pStandardItemModel_->appendRow(item);
    }

    pListView_->setModel(pStandardItemModel_);

    if (0 == pStandardItemModel_->rowCount())
    {
        return;
    }

    // 瀹屾垚鍒楄〃鐨勫畾浣?    pListView_->setMinimumWidth(width());
    pListView_->setMaximumWidth(width());

    QPoint p(0, height());
    int x = mapToGlobal(p).x();
    int y = mapToGlobal(p).y() + 1;

    pListView_->move(x, y);
    pListView_->show();
	this->setFocus();
}

void CompleteLineEdit::slotCompleteText(const QModelIndex &index)
{
    QString name = index.data().toString();
    setText(name);

    emit signalSelectItem(name);

//    pListView_->hide();
	this->hide();
}

void CompleteLineEdit::hide()
{
    QLineEdit::hide();
    if(!pListView_->isHidden())
	{
        pListView_->hide();
    }
	pListView_->clearFocus();
	this->clearFocus();
}

void CompleteLineEdit::addModulesData(const QMap<QString, QString> &data)
{
	map_moduleName_moduleTag_ = data;
}

void CompleteLineEdit::show()
{
	qDebug() << "璁剧疆鐒︾偣";
	QLineEdit::show();
	if (text().isEmpty())
	{
		pListView_->hide();
	}
	this->setFocus();
}


