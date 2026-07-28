#　アプリケーション制御層関数仕様書
## メイン処理
### [1] main.c 
### [2] 関数仕様


#### parse_target_count()
- main()のコマンドライン引数を計算しやすい形にする．
- main()の引数のエラー処理
- char型 => long型 => int型
- 引数
    - argc: コマンドライン引数の数
    - *argv[]: コマンドライン引数
    - *terget_count
        - int型に直すポインタ変数
        - main()内で定義
- 戻り値
    - 正常終了時はゼロ 
    - 引数または引数の個数が不正で -1



##### main()

- パンの製造目標数を決める
- controller.h内のRunProduction()を呼び出して製造を実行
- 引数
    - argc : コマンドライン引数の数
    - *argv[] : コマンドライン引数

- 戻り値
    - 正常終了でゼロ
    - 製造エラーで 1 
    - 引数が不正で 2




## 関数呼び出し部
### [1] controller.c
### [2] 関数仕様

#### move_conveyor()

- コンベアを目的座標まで移動させる
- オーバーシュート対策で最大移動距離は1.0
- 引数
    - target : 目的とする座標

- 戻り値
    - 正常終了でゼロ
    - 異常発生で-1


#### should_report_retry()
 - ログを間引く関数
 - retry_positioned_operation()で呼び出される
 - 引数
    - attempt : retry_positioned_operation()の実行カウンタ
    - 

#### retry_positioned_operation()

- 測定ノイズ対策で同じ関数を100回呼び出す
- 引数
    - position : 目標座標
    - operation : 関数ポインタを渡す




