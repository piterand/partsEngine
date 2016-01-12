#include "honeycombspinicearray.h"

HoneycombSpinIceArray::HoneycombSpinIceArray()
{
     _type = "honeycomb";
     SysLoader::reg<HoneycombSpinIceArray>(type());
}

HoneycombSpinIceArray::HoneycombSpinIceArray(const HoneycombSpinIceArray &sys) :
PartArray(sys)
{
    //копируем содержание ячеек
    vector<HoneycombSpinIceCell*>::const_iterator iter = sys.cells.begin();
    HoneycombSpinIceCell *tempCell, *oldCell;
    while(iter!=sys.cells.end()){
        tempCell = new HoneycombSpinIceCell();
        oldCell = *iter;
        for (int j=0;j<6;j++)
            for (int i=0;i<this->count();i++){
                if (*(oldCell->parts[j])==*(this->parts[i])){
                    tempCell->parts.push_back(this->parts[i]);
                    break;
                }
            }

        tempCell->pos = oldCell->pos;
        this->cells.push_back(tempCell);
        iter++;
    }
    this->m = sys.m;
    this->n = sys.n;
}

HoneycombSpinIceArray::~HoneycombSpinIceArray()
{
    this->clear();
}

HoneycombSpinIceArray &HoneycombSpinIceArray::operator =(const HoneycombSpinIceArray &sys)
{
    if (this == &sys) return *this;
    this->PartArray::operator =((PartArray)sys);
    //копируем содержание ячеек
    vector<HoneycombSpinIceCell*>::const_iterator iter = sys.cells.begin();
    HoneycombSpinIceCell *tempCell, *oldCell;
    while(iter!=sys.cells.end()){
        tempCell = new HoneycombSpinIceCell();
        oldCell = *iter;
        for (int j=0;j<6;j++)
            for (int i=0;i<this->count();i++){
                if (oldCell->parts[j]==this->parts[i]){
                    tempCell->parts.push_back(this->parts[i]);
                    break;
                }
            }

        tempCell->pos = oldCell->pos;
        this->cells.push_back(tempCell);
        iter++;
    }
    this->m = sys.m;
    this->n = sys.n;
    return *this;
}

void HoneycombSpinIceArray::dropHoneyComb(int m, int n, double a, Part *tmp)
{
    this->clear();

    this->m = m; this->n = n;
    double mLength=0; //магнитный момент одной частицы
    if (tmp==0){ //если шаблон частицы не был передан, делаем шаблон по умолчанию
        tmp = new Part();
        mLength = config::Instance()->m;
    } else {
        mLength = tmp->m.length();
    }

    double r=a*sqrt(3)/2;
    for(int j=0;j<n;j++)
    {
        for(int i=0;i<m;i++)
        {
            double x=2*r*i+r*(pow(-1,j)+1)/2;
            double y=sqrt(3*r*r)*j;

            HoneycombSpinIceCell *tempCell = new HoneycombSpinIceCell();
            tempCell->pos = Vect(x,y,1);

            vector<Part*> hexPart;

            //genHexPart start
            for(int k=0;k<6;k++)
            {
                Part* temp = new Part(*tmp);
                temp->pos.x = r*cos(2*M_PI*k/6)+x;
                temp->pos.y = r*sin(2*M_PI*k/6)+y;
                temp->m.x = cos(2*M_PI*k/6+M_PI/2)*mLength;
                temp->m.y = sin(2*M_PI*k/6+M_PI/2)*mLength;
                hexPart.push_back(temp);
            }
            //genHexPart end

            vector<Part*>::iterator iter = hexPart.begin();
            while (iter!=hexPart.end()){
                bool add=true;
                vector<Part*>::iterator iter2 = this->parts.begin();
                while(iter2!=this->parts.end()){
                    if (
                            this->_double_equals((*iter2)->pos.x,(*iter)->pos.x) &&
                            this->_double_equals((*iter2)->pos.y,(*iter)->pos.y)
                            )
                    {
                        add=false;
                        delete (*iter);//удаляем из памяти
                        (*iter) = (*iter2);
                        break;
                    }
                    iter2++;
                }

                if(add)
                    this->insert(*iter);

                tempCell->parts.push_back(*iter);

                iter++;
            }
            this->cells.push_back(tempCell);
        }
    }
}

StateMachineFree HoneycombSpinIceArray::groundState()
{
    HoneycombSpinIceCell* temp;
    for (int i=0;i<n;i++)
        for (int j=0;j<m;j++){
            temp = cells[i*m+j];
            switch ((j+(i%2))%3){
            case 0:
                temp->rotateClockWise();
                break;
            case 1:
                temp->rotateAntiClockWise();
                break;
            case 2:
                temp->rotateChaotic();
                break;
            }
        }
    return state;
}

StateMachineFree HoneycombSpinIceArray::maximalState()
{
    for (int i=0;i<m*n;i++){
        cells[i]->rotateChaotic();
    }
    return state;

}

void HoneycombSpinIceArray::clear()
{
    this->clearCells();

    PartArray::clear();
}

void HoneycombSpinIceArray::load(QString file)
{
    this->clear();
    //load base part of file
    PartArray::load(file);

    //open file
    QFile infile(file);
    infile.open(QFile::ReadOnly);
    QTextStream f(&infile);

    //skip to cells section
    QString s;
    while (s!="[cells]"){
        s = f.readLine();
    }


    //read particles data
    s=f.readLine();

    QStringList params;
    HoneycombSpinIceCell *tempCell;
    while (! (
               (s[0]=='[' && s[s.length()-1]==']') ||
               (s.isEmpty())
               )){ //read due to the next section or end of file
        params = s.split('\t');
        tempCell = new HoneycombSpinIceCell();
        tempCell->pos = Vect(
                    params[0].toDouble(),
                    params[1].toDouble(),
                    1
                    );
        if (tempCell->pos.y==0)
            m++;
        for (int i=2;i<8;i++)
            tempCell->parts.push_back(this->getById(i));

        this->cells.push_back(tempCell);

        s=f.readLine();
    }

    //close file
    infile.close();

    n = this->cells.size()/m;
}

void HoneycombSpinIceArray::save(QString file)
{
    //save base part of file
    PartArray::save(file);

    //open file in append mode
    QFile outfile(file);
    outfile.open(QFile::WriteOnly | QFile::Append);
    QTextStream f(&outfile);

    //write header
    f<<"[cells]"<<endl;

    //write particles
    vector<HoneycombSpinIceCell*>::iterator iter = this->cells.begin();
    while (iter != this->cells.end()) {
        vector<Part*>::iterator iter2 = (*iter)->parts.begin();
        f<<(*iter)->pos.x<<"\t"<<(*iter)->pos.y;
        while (iter2 != (*iter)->parts.end()){
            f<<"\t"<<(*iter2)->Id();
            iter2++;
        }
        f << endl;
        iter++;
    }

    //close file
    outfile.close();
}

void HoneycombSpinIceArray::clearCells()
{
    vector<HoneycombSpinIceCell*>::iterator iter = this->cells.begin();
    while (iter!=this->cells.end()){
        delete (*iter); //удаляем из памяти все ячейки
        iter++;
    }
    this->cells.clear();
    this->m = this->n = 0;
}

