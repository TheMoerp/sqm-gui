#include "sqmtablemodel.h"
#include <algorithm>
#include <QColor>


SQMTableModel::SQMTableModel(QObject *parent)
    : QAbstractTableModel(parent) {
}


int SQMTableModel::rowCount(const QModelIndex & /*parent*/) const {
    return binLen;
}

int SQMTableModel::columnCount(const QModelIndex & /*parent*/) const {
    return 3;
}


QVariant SQMTableModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();
    QVariant result;
    switch (role) {
    case Qt::DisplayRole:

        try {
            result = QString::number(sqmMatrix.at(col).at(row));
        } catch (...) {
            result = "x";
        }

        break;
    case Qt::ForegroundRole:
        if (changedHere.isValid()) {


            if ((row > changedHere.row() || (col >= changedHere.column() && row == changedHere.row())) && (col != 0 || changedHere.column() == 0)) {
                result = QColor(Qt::red);
            }
        }
        break;
    }

//    if (role == Qt::DisplayRole) {
//        int row = index.row();
//        int col = index.column();
//        QString result;
//        try {
//            result = QString::number(sqmMatrix.at(col).at(row));
//        } catch (...) {
//            result = "x";
//        }

//        return result;
//    }
    return result;

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
        changedHere = index;

        UpdateSqmMatrix(index);
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

    //changedHere.model()->index(-1, -1, QModelIndex());
    CalculateSqmMatrix();
}


// Convert int to binary
std::string SQMTableModel::IntToBinary(int n) {
    string bin;
    int mask = 1;
    while (n != 0) {
        bin += (n & mask) == 0 ? "0" : "1";
        n = n >> 1;
    }

    reverse(bin.begin(), bin.end());
    return bin;
}

void SQMTableModel::CalculateSqmMatrix() {
    // Clear sqmMatrix
    sqmMatrix.clear();

    // Calculate binary of exponent
    string bin = IntToBinary(exp);
    binLen = bin.length();

    // Init BIN Column
    vector<int> colBin;
    for (int i = 0; i < binLen; i++) {
        colBin.push_back(bin[i] - '0');
    }

    // Add or remove rows
    beginResetModel();
    sqmMatrix.push_back(colBin);
    endResetModel();

    // Init SQN & MUL Column
    vector<int> colSqn, colMul;
    colSqn.push_back(1);
    colMul.push_back(base);
    sqmMatrix.push_back(colSqn);
    sqmMatrix.push_back(colMul);


    // Calculate SQM
    for (int i = 1; i < binLen; i++) {
        sqmMatrix.at(1).push_back((sqmMatrix.at(2).at(i - 1) * sqmMatrix.at(2).at(i - 1)) % mod);

        if (sqmMatrix.at(0).at(i) == 0) {
            sqmMatrix.at(2).push_back(sqmMatrix.at(1).at(i));
        }
        else {
            sqmMatrix.at(2).push_back((sqmMatrix.at(1).at(i) * base) % mod);
        }
    }
}


void SQMTableModel::UpdateSqmMatrix(QModelIndex startIndex) {
    int start_row = startIndex.row();
    int start_col = startIndex.column();

    // If change appeared in col 2, start in next row
    if (start_col == 2) {
        start_row++;
    }

    // Update sqmMatrix
    for (int i = start_row; i < binLen; i++) {
        if (start_col != 1) {
            sqmMatrix.at(1).at(i) = (sqmMatrix.at(2).at(i - 1) * sqmMatrix.at(2).at(i - 1)) % mod;

        }
        start_col = 0;
        if (sqmMatrix.at(0).at(i) == 0) {
            sqmMatrix.at(2).at(i) = sqmMatrix.at(1).at(i);
        }
        else {
            sqmMatrix.at(2).at(i) = (sqmMatrix.at(1).at(i) * base) % mod;
        }
    }
}
