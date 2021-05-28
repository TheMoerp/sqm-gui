#include "sqmtablemodel.h"


SQMTableModel::SQMTableModel(QObject *parent)
    : QAbstractTableModel(parent) {
}


int SQMTableModel::rowCount(const QModelIndex & /*parent*/) const {
    return 2;
}

int SQMTableModel::columnCount(const QModelIndex & /*parent*/) const {
    return 3;
}


QVariant SQMTableModel::data(const QModelIndex &index, int role) const {

    if (role == Qt::DisplayRole) {
        int row = index.row();
        int col = index.column();

        return sqmMatrix.at(col).at(row);
    }
    return QVariant();
}

QVariant SQMTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QString("BIN");
            case 1:
                return QString("SQN");
            case 2:
                return QString("MUL");
            }
        }
        return QVariant();
}


bool SQMTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        if (!checkIndex(index)) {
            return false;
        }
        int row = index.row();
        int col = index.column();


        // call calculateSqmMatrix
        sqmMatrix.at(col).at(row) = value.toInt();
        CalculateSqmMatrix(row);
        return true;
    }
    return false;
}


Qt::ItemFlags SQMTableModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}



void SQMTableModel::SetStartValues(int pBase, int pExp, int pMod) {
    base = pBase;
    exp = pExp;
    mod = pMod;
}


// Convert int to binary
std::string SQMTableModel::IntToBinary(int n) {
    string bin;
    int mask = 1;
    while (n != 0) {
        bin += (n & mask) == 0 ? "0" : "1";
        n = n >> 1;
    }

    return bin;
}

void SQMTableModel::CalculateSqmMatrix(int startRow) {
    string bin = IntToBinary(exp);
    binLen = bin.length();

    // Initialize bin column
    vector<int> colBin;
    for (int i = 0; i < binLen; i++) {
        colBin.push_back(bin[i] - '0');
    }
    sqmMatrix.at(0) = colBin;

    for (int i = startRow; i < binLen; i++) {
        if (i == 0) {
            // Initialize first row
            sqmMatrix.at(1).at(0) = 1;
            sqmMatrix.at(2).at(0) = base;
        }
        else {
            // Square Multiply
            sqmMatrix.at(1).at(i) = sqmMatrix.at(2).at(i - 1) * sqmMatrix.at(2).at(i - 1);

            if (sqmMatrix.at(0).at(i) == 0) {
                sqmMatrix.at(2).at(i) = sqmMatrix.at(1).at(i);
            }
            else {
                sqmMatrix.at(2).at(i) = sqmMatrix.at(1).at(i) * base;
            }
        }
    }
}
