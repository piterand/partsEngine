#ifndef PARTARRAYTEST2D_H
#define PARTARRAYTEST2D_H

#include <QObject>
#include <QTest>
#include <QDebug>
#include "PartArray.h"
#include "config.h"
#include "random.h"
#include "ctime"

class PartArrayTest2D : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase(){
        config::Instance()->set2D();
        Random::Instance();
    }
    void dropRandomI()
    {
        PartArray *a = new PartArray(10,10,1);
        a->dropRandom(10);
        QCOMPARE(a->count(),10);
        a->dropRandom(1);
        QCOMPARE(a->count(),1);
        delete a;
    }
    void dropRandomD(){
        PartArray *a = new PartArray(10,10,1);
        a->dropRandom(0.3);
        QVERIFY(a->destiny(true)>0.25&&a->destiny(true)<0.35);
        a->dropRandom(0.4);
        QVERIFY(a->destiny(true)>0.35&&a->destiny(true)<0.45);
        delete a;
    }

    void energy(){
        //тест вычисления энергии после создания системы
        PartArray *sys = new PartArray(10,10,10);
        sys->dropRandom(20);

        QCOMPARE(sys->E(), sys->EComplete());

        //после встряхивания состояния
        sys->state->randomize(10);
        QCOMPARE(sys->E(), sys->EComplete());

        //после обнуления конфигурации
        sys->state->reset();
        sys->state->randomize(10);
        QCOMPARE(sys->E(), sys->EComplete());

        //после полного обнуления конфигурации
        sys->state->hardReset();
        QCOMPARE(sys->E(), sys->EComplete());

    }

};

#endif // PARTARRAYTEST2D_H
