//
// Created by sumida on 2017/01/01.
//

#ifndef GA_GA_HPP_HPP
#define GA_GA_HPP_HPP

#include <random>
#include <limits>
#include <vector>

template <class T> class GA
{
private:

    /**
     * とりうる最大値
     */
    T _max;

    /**
     * とりうる最小値
     */
    T _min;

    /**
     * 子の数
     */
    int _lamda;

    /**
     * 親の数
     */
    int _mu;

    /**
     * データの長さ
     */
    int _length;

    /**
     * 子の配列
     */
    T** _children;

    /**
     * 親の配列
     */
    T** _parent;

    /**
     * 世代数
     */
    int _gene;

    /**
     * 子の評価値とインデックスを保持する配列
     * [i][0] = 評価値, [i][1] = インデックス
     */
    double** _eval_children;

    /**
     * 親の評価値とインデックスを保持する配列
     * [i][0] = 評価値, [i][1] = インデックス
     */
    double** _eval_parent;

    /**
     * 大きければいいのか、小さければいいのか？のフラグ
     */
    int _eval_flag;

    /**
     * アルゴリズムの選択フラグ
     */
    int _algf_flag;

    /**
     * データの種類選択フラグ
     */
    int _value_flag;

    /**
     * max,minの範囲内で実数値乱数で初期化
     */
    void _random_init_real(T** data,int index1_size,int index2_size)
    {
        std::random_device rnd;
        std::mt19937 mt(rnd());
        std::uniform_real_distribution<> rand100(_min, _max);

        for(int i = 0;i < index1_size;i++)
        {
            for(int j = 0;j < index2_size;j++)
            {
                data[i][j] = rand100(mt);
            }
        }
    }

    /**
     * 0or1で初期化
     */
    void _random_init_bit(T** data,int index1_size,int index2_size)
    {
        std::random_device rnd;
        std::mt19937 mt(rnd());
        std::uniform_real_distribution<> rand100(0.0, 1.0);

        for(int i = 0;i < index1_size;i++)
        {
            for(int j = 0;j < index2_size;j++)
            {
                if(rand100(mt) > 0.5)
                {
                    data[i][j] = 1;
                }else
                {
                    data[i][j] = 0;
                }

            }
        }
    }

    /**
     * 親・子配列とそれらの評価値配列の初期化メソッド
     */
    void _init_array()
    {
        std::random_device rnd;
        std::mt19937 mt(rnd());
        std::uniform_real_distribution<> rand100(_min, _max);

        _children = new T*[_lamda];
        _parent = new T*[_mu];
        _eval_children = new double*[_lamda];
        _eval_parent = new double*[_mu];

        //子配列を乱数で初期化
        for (int i = 0; i < _lamda; ++i) {
            _children[i] = new T[_length];
            _eval_children[i] = new double[2];
//            for(int j = 0;j < _length;j++)
//            {
//                _children[i][j] = rand100(mt);
//            }


            switch (_eval_flag)
            {
                case SMALL:
                    _eval_children[i][0] = std::numeric_limits<T>::max();
                    break;
                case LARGE:
                    _eval_children[i][0] = std::numeric_limits<T>::min();
                    break;
                default:
                    break;
            }
            _eval_children[i][0] = i;

        }

        //親配列を乱数で初期化
        for (int i = 0; i < _mu; ++i) {
            _parent[i] = new T[_length];
            _eval_parent[i] = new double[2];
//            for(int j = 0;j < _length;j++)
//            {
//                _parent[i][j] = rand100(mt);
//            }

            switch (_eval_flag)
            {
                case SMALL:
                    _eval_parent[i][0] = std::numeric_limits<T>::max();
                    break;
                case LARGE:
                    _eval_parent[i][0] = std::numeric_limits<T>::min();
                    break;
                default:
                    break;
            }
            _eval_parent[i][0] = i;
        }

        switch (_value_flag)
        {
            case REAL:
                _random_init_real(_parent,_mu,_length);
                _random_init_real(_children,_lamda,_length);
                break;

            case BIT:
                _random_init_bit(_parent,_mu,_length);
                _random_init_bit(_children,_lamda,_length);
                break;

            default:
                break;
        }


    }

    /**
     * 昇順or降順に_eval_childをソートする
     */
    void _sort_children()
    {
        for(int i= 0;i < _lamda;i++)
        {
            for(int j = i;j < _lamda;j++)
            {
                switch (_eval_flag)
                {
                    case LARGE:
                        if(_eval_children[i][0] < _eval_children[j][0])
                        {
                            T tmp1 = _eval_children[j][0];
                            T tmp2 = _eval_children[j][1];

                            _eval_children[j][0] = _eval_children[i][0];
                            _eval_children[j][1] = _eval_children[i][1];

                            _eval_children[i][0] = tmp1;
                            _eval_children[i][1] = tmp2;
                        }
                        break;

                    case SMALL:
                        if(_eval_children[i][0] > _eval_children[j][0])
                        {
                            T tmp1 = _eval_children[j][0];
                            T tmp2 = _eval_children[j][1];

                            _eval_children[j][0] = _eval_children[i][0];
                            _eval_children[j][1] = _eval_children[i][1];

                            _eval_children[i][0] = tmp1;
                            _eval_children[i][1] = tmp2;
                        }
                        break;
                }
            }
        }

    }

    /**
     * 淘汰処理
     */
    void _selection()
    {
        //親用のtmp配列を生成
        T** tmp_parent = new T*[_mu];
        T** tmp_eval = new T*[_mu];
        //選出時に用いるindex
        int parent_index = 0;
        int child_index = 0;

        //tmp配列を初期化
        for(int i = 0;i < _mu;i++)
        {
            tmp_parent[i] = new T[_length];
            tmp_eval[i] = new T[2];
        }


        //入力されてきた評価値に対してソートを掛ける
        _sort_children();

        //親と子の間で最も評価値が良いものを選んで、親配列に格納
        for(int i= 0;i < _mu;i++)
        {
            switch (_eval_flag){
                case LARGE:
                    if(_eval_parent[parent_index][0] > _eval_children[child_index][0])
                    {
                        for(int j = 0;j < _length;j++)
                        {
                            tmp_parent[i][j] = _parent[i][j];
                        }
                        tmp_eval[i][0] = _eval_parent[i][0];
                        tmp_eval[i][1] = i;
                        parent_index+=1;
                    }else
                    {
                        for(int j = 0;j < _length;j++)
                        {
                            tmp_parent[i][j] = _children[i][j];
                        }
                        tmp_eval[i][0] = _eval_children[i][0];
                        tmp_eval[i][1] = i;
                        child_index+=1;
                    }
                    break;

                case SMALL:
                    if(_eval_parent[parent_index][0] < _eval_children[child_index][0])
                    {
                        for(int j = 0;j < _length;j++)
                        {
                            tmp_parent[i][j] = _parent[i][j];
                        }
                        tmp_eval[i][0] = _eval_parent[i][0];
                        tmp_eval[i][1] = i;
                        parent_index+=1;
                    }else
                    {
                        for(int j = 0;j < _length;j++)
                        {
                            tmp_parent[i][j] = _children[i][j];
                        }
                        tmp_eval[i][0] = _eval_children[i][0];
                        tmp_eval[i][1] = i;
                        child_index+=1;
                    }
                    break;

                default:
                    break;
            }
        }


        //tmp配列に入った親データをすべてparent配列に格納
        for(int i = 0;i < _mu;i++)
        {
            for(int j = 0;j < _length;j++)
                _parent[i][j] = tmp_parent[i][j];
            _eval_parent[i][0] = tmp_eval[i][0];
            _eval_parent[i][1] = tmp_eval[i][1];
        }

        //配列の解放
        for(int i = 0;i < _mu;i++)
        {
            if(tmp_parent[i]) delete [] tmp_parent[i];
            if(tmp_eval[i]) delete [] tmp_eval[i];
        }
        delete [] tmp_parent;
        delete [] tmp_eval;
    }

    /**
     * ランダムな2つのインデックスを生成
     */
    void _make_rand_index(int* index,int max,int min)
    {
        std::random_device rnd;
        std::mt19937 mt(rnd());
        std::uniform_real_distribution<> rand100(min, max);

        index[0] = rand100(mt);
        index[1] = index[0];

        while(index[0] == index[1])
        {
            index[1] = rand100(mt);
        }

        if(index[0] > index[1])
        {
            int tmp = index[0];
            index[0] = index[1];
            index[1] = tmp;
        }
    }

    /**
     * 突然変異
     */
    void _mutation(T** data,int index1,int index2)
    {
        std::random_device rnd;
        std::mt19937 mt(rnd());
        std::uniform_real_distribution<> rand100(0.0, 1.0);

        std::random_device rnd2;
        std::mt19937 mt2(rnd());
        std::uniform_real_distribution<> rand200(_min, _max);

        double rate = rand100(mt);
        if(rate < 0.01)
        {
            switch (_value_flag)
            {
                case REAL:
                    data[index1][index2] = rand200(mt2);
                    if(_max < data[index1][index2])
                        data[index1][index2] = _max;
                    if(_min > data[index1][index2])
                        data[index1][index2] = _min;
                    break;

                case BIT:
                    if(data[index1][index2] == 1)
                    {
                        data[index1][index2] == 0;
                    }else
                    {
                        data[index1][index2] == 1;
                    }
                    break;

                default:
                    break;
            }
        }
    }

    /**
     * 一点交叉
     */
    void _crossover_one_point()
    {
        std::random_device rnd;
        std::mt19937 mt(rnd());
        std::uniform_real_distribution<> rand100(0, _length);
        int* index = new int[2];
        int cross_point;

        for(int i= 0;i < _lamda;i+=2)
        {
            _make_rand_index(index,0,_mu);
            cross_point = rand100(mt);

            for(int j = 0;j < _length;j++)
            {
                if(j < cross_point)
                {
                    _children[i][j] = _parent[index[0]][j];
                    _children[i+1][j] = _parent[index[1]][j];
                }else{
                    _children[i][j] = _parent[index[1]][j];
                    _children[i+1][j] = _parent[index[0]][j];
                }
                _mutation(_children,i,j);
                _mutation(_children,i+1,j);
            }
        }
    }

    /**
     * 2点交叉
     */
    void _crossover_two_point()
    {

        int* index = new int[2];
        int* cross_point = new int[2];

        for(int i= 0;i < _lamda;i+=2)
        {
            _make_rand_index(index,0,_mu);
            _make_rand_index(cross_point,0,_length);

            for(int j = 0;j < _length;j++)
            {
                if (i >= cross_point[1]) {
                    _children[i][j] = _parent[index[0]][j];
                    _children[i + 1][j] = _parent[index[1]][j];
                }
                else {
                    if (j < cross_point[0]) {
                        _children[i][j] = _parent[index[0]][j];
                        _children[i + 1][j] = _parent[index[1]][j];
                    } else {
                        _children[i][j] = _parent[index[1]][j];
                        _children[i + 1][j] = _parent[index[0]][j];
                    }
                }
                _mutation(_children,i,j);
                _mutation(_children,i+1,j);
            }
        }
    }

    /**
     * 一様交叉
     */
    void _crossover_uniform()
    {
        std::random_device rnd;
        std::mt19937 mt(rnd());
        std::uniform_real_distribution<> rand100(0.0, 1.0);
        int* index = new int[2];

        for(int i = 0;i < _lamda;i+=2)
        {
            _make_rand_index(index,0,_mu);
            for(int j = 0;j < _length;j++)
            {
                if(rand100(mt) > 0.5)
                {
                    _children[i][j] = _parent[index[0]][j];
                    _children[i+1][j] = _parent[index[1]][j];
                }else
                {
                    _children[i][j] = _parent[index[1]][j];
                    _children[i+1][j] = _parent[index[0]][j];
                }
                _mutation(_children,i,j);
                _mutation(_children,i+1,j);
            }
        }

    }

    /**
     * ブレンド交叉
     */
    void _crossover_blend()
    {

    }


public:
    /**
     * 評価値の基準
     * 大きい方が評価が良いならLARGE
     * 小さい方が評価が良いならSMALL
     */
    enum {LARGE,SMALL};

    /**
     * GAアルゴリズムにおける交叉アルゴリズムの選択
     */
    enum {ONE_POINT,TWO_POINT,UNIFORM,BLEND};

    /**
     * 取り扱うデータの種類
     * 実数値 or 1bit
     */
    enum {REAL,BIT};

    /**
     * コンストラクタ
     */
    GA(int lamda,int mu,int length,T max=1.0,T min=0.0,int eval_flag=LARGE,int algf_flag=ONE_POINT,int value_flag=BIT)
    {
        _lamda = lamda;
        _mu =mu;
        _max = max;
        _min = min;
        _length = length;
        _gene = 1;
        _eval_flag = eval_flag;
        _algf_flag = algf_flag;
        _value_flag = value_flag;
        if(_algf_flag == BLEND && _value_flag == BIT)
        {
            std::cout << "ERROR" << std::endl;
            std::cout << "CHANGE-> ALGF=ONE_POINT" << std::endl;
            _algf_flag = ONE_POINT;
        }
        _init_array();
    }

    /**
     * デストラクタ
     */
    ~GA()
    {
        if(_eval_children)
        {
            for(int i= 0;i < _lamda;i++)
                if(_eval_children[i]) delete [] _eval_children[i];
            delete [] _eval_children;
        }

        if(_eval_parent)
        {
            for(int i=0;i < _mu;i++)
                if(_eval_parent[i]) delete [] _eval_parent[i];
            delete [] _eval_parent;
        }

        if(_children)
        {
            for(int i = 0;i < _lamda;i++)
                if(_children[i]) delete [] _children[i];
            delete [] _children;
        }

        if(_parent)
        {
            for(int i = 0;i < _mu;i++)
                if(_parent[i]) delete [] _parent[i];
            delete [] _parent;
        }

    }

    /**
     * 与えられた評価値から新しく子供を生成する関数
     */
    void _calc_ga(T* eval_value)
    {
        try {
            //評価値をコピー
            for(int i = 0;i < _lamda;i++) {
                _eval_children[i][0] = eval_value[i];
                _eval_children[i][1] = i;
            }

            switch (_algf_flag)
            {
                case ONE_POINT:
                    break;

                case TWO_POINT:
                    break;

                case UNIFORM:
                    break;

                case BLEND:
                    break;

                default:
                    break;
            }

            _gene += 1;

        }catch (...)
        {
            std::cout << "Exception" << std::endl;
        }


    }

    /**
     * 子配列のゲッター
     */
    T** _get_children()
    {
        return _children;
    }

    /**
     * 現在の世代数のゲッター
     */
    int _get_gene()
    {
        return _gene;
    }


};

#endif //GA_GA_HPP_HPP
