#ifndef LRC_H
#define LRC_H

#include "parser.h"

class LRC:public Parser
{
   // Q_OBJECT
public:
    LRC(QWidget *parent=nullptr);
    virtual ~LRC();

    virtual void analyzeData() override;


protected:
    virtual const std::multimap<QByteArray, QByteArray> & create_dictionary() override;
    virtual QByteArray analyzeMessage(QByteArray &) override;

    const std::multimap<QByteArray, QByteArray>& dictionary=create_dictionary();

};

#endif // LRC_H
