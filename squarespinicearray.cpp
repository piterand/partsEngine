#include "squarespinicearray.h"

SquareSpinIceArray::SquareSpinIceArray()
{

}

SquareSpinIceArray::~SquareSpinIceArray()
{
    this->clearCells();
}

void SquareSpinIceArray::dropSpinIce(int hCells, int vCells, double l)
{
    if (config::Instance()->dimensions()!=2)
        return;
    double l_2 = l/2.; //полурешетка
    Vect center(0,0,0);

    Part *temp;
    Vect partPos(0,0,0);
    center.y = l_2;
    for (int i=0;i<vCells;i++){
        center.x = l_2;
        for (int j=0;j<hCells;j++){
            //создаем ячейку
            SquareSpinIceCell *cell = new SquareSpinIceCell();
            cell->pos = center;

            //добавляем верхнюю
            partPos.x = center.x;
            partPos.y = center.y+l_2;

            if ((temp = this->findByPosition(partPos)) == 0){
                temp = new Part();
                temp->shape = Part::ELLIPSE;
                temp->pos = partPos;
                temp->m = Vect(config::Instance()->m,0,0);
                this->insert(temp);
            }
            cell->top = temp;

            //добавляем нижнюю
            partPos.x = center.x;
            partPos.y = center.y-l_2;

            if ((temp = this->findByPosition(partPos)) == 0){
                temp = new Part();
                temp->shape = Part::ELLIPSE;
                temp->pos = partPos;
                temp->m = Vect(config::Instance()->m,0,0);
                this->insert(temp);
            }
            cell->bottom = temp;

            //добавляем левую
            partPos.x = center.x-l_2;
            partPos.y = center.y;

            if ((temp = this->findByPosition(partPos)) == 0){
                temp = new Part();
                temp->shape = Part::ELLIPSE;
                temp->pos = partPos;
                temp->m = Vect(0,config::Instance()->m,0);
                this->insert(temp);
            }
            cell->left = temp;

            //добавляем правую
            partPos.x = center.x+l_2;
            partPos.y = center.y;

            if ((temp = this->findByPosition(partPos)) == 0){
                temp = new Part();
                temp->shape = Part::ELLIPSE;
                temp->pos = partPos;
                temp->m = Vect(0,config::Instance()->m,0);
                this->insert(temp);
            }
            cell->right = temp;
            cell->row = i;
            cell->column = j;

            this->cells.push_back(cell);

            center.x+=l;
        }
        center.y+=l;
    }
}

Part *SquareSpinIceArray::findByPosition(const Vect &pos, double epsilon)
{
    vector<Part*>::reverse_iterator iter = this->parts.rbegin();
    Part* temp;
    while (iter!=this->parts.rend()){
        temp = *iter;
        if (fabs(temp->pos.x-pos.x)<epsilon && fabs(temp->pos.y-pos.y)<epsilon){
            return temp;
        }
        iter++;
    }
    return 0;
}

bool SquareSpinIceArray::setToGroundState()
{
    vector<SquareSpinIceCell*>::iterator iter = this->cells.begin();
    double m = config::Instance()->m;
    SquareSpinIceCell *temp;
    while (iter!=this->cells.end()){
        temp = *iter;
        int direction = (temp->row+temp->column)%2*2-1; //либо +1 либо -1, в шахматном порядке
        temp->top->m = Vect(m*direction,0,0);
        temp->bottom->m = Vect(m*direction*-1.,0,0);
        temp->left->m = Vect(0,m*direction,0);
        temp->right->m = Vect(0,m*direction*-1.,0);

        iter++;
    }
    return true;
}

bool SquareSpinIceArray::setToMaximalState()
{
    vector<SquareSpinIceCell*>::iterator iter = this->cells.begin();
    double m = config::Instance()->m;
    SquareSpinIceCell *temp;
    while (iter!=this->cells.end()){
        temp = *iter;
        int direction = (temp->row+temp->column)%2*2-1;
        temp->top->m = Vect(m*direction,0,0);
        temp->bottom->m = Vect(m*direction*-1.,0,0);
        temp->left->m = Vect(0,m*direction*-1.,0);
        temp->right->m = Vect(0,m*direction,0);

        iter++;
    }
    return true;
}

SquareSpinIceArray *SquareSpinIceArray::copy()
{
    SquareSpinIceArray *temp = new SquareSpinIceArray();
    temp->E1 = this->E1;
    temp->E2 = this->E2;
    temp->eIncrementalTemp = this->eIncrementalTemp;
    temp->size = this->size;
    temp->absSize = this->absSize;

    //копируем частицы
    vector<Part*>::iterator iter1 = this->parts.begin();
    while(iter1!=this->parts.end()){
        temp->insert((*iter1)->copy());
        iter1++;
    }

    //копируем содержание ячеек
    vector<SquareSpinIceCell*>::iterator iter = this->cells.begin();
    SquareSpinIceCell *tempCell, *oldCell;
    while(iter!=this->cells.end()){
        tempCell = new SquareSpinIceCell();
        oldCell = *iter;
        for (int i=0;i<this->count();i++){
            if (oldCell->top==this->parts[i])
                tempCell->top=temp->parts[i];

            if (oldCell->right==this->parts[i])
                tempCell->right=temp->parts[i];

            if (oldCell->bottom==this->parts[i])
                tempCell->bottom=temp->parts[i];

            if (oldCell->left==this->parts[i])
                tempCell->left=temp->parts[i];
        }

        tempCell->column = oldCell->column;
        tempCell->row = oldCell->row;
        tempCell->pos = oldCell->pos;
        temp->cells.push_back(tempCell);
        iter++;
    }

    return temp;
}

void SquareSpinIceArray::clearCells()
{
    vector<SquareSpinIceCell*>::iterator iter = this->cells.begin();
    while (iter!=this->cells.end()){
        delete (*iter); //удаляем все что по есть по ссылкам на частицы
        iter++;
    }
    this->cells.clear();
}

