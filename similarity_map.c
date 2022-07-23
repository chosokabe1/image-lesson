/*
 * 画像処理・理解特論
 * モデルヒストグラムとの類似度を画像化
 *
 * Time-stamp: <07/10/12 11:39:22 kinoshita>
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH           640     // 画像の横画素数
#define HEIGHT          480     // 画像の縦画素数
#define MODEL_WIDTH     67      // モデルの横画素数（領域の対称性より奇数に限定）
#define MODEL_HEIGHT    99      // モデルの縦画素数（領域の対称性より奇数に限定）
#define RADIUS_WIDTH    33      // 領域の横半径 (モデルの横画素数 - 1) / 2
#define RADIUS_HEIGHT   49      // 領域の縦半径 (モデルの縦画素数 - 1) / 2
#define Q_STEP          16      // 量子化間隔
#define Q_SIZE          16      // 量子化サイズ（RGB：256 / Q_STEP）
#define NBIN            4096    // ヒストグラムのビン数（RGB：Q_SIZE * Q_SIZE * QSIZE）

// 以下，二つの関数を実装
void calc_histogram(double hist[], unsigned char image[][MODEL_WIDTH][3],
                    double kernel[][MODEL_WIDTH]);
double calc_bhattacharrya(double hist1[], double hist2[]);
/////////////////////////

void set_kernel(double kernel[][MODEL_WIDTH]);
void copy_region(unsigned char src[][WIDTH][3], unsigned char dst[][MODEL_WIDTH][3],
                 int upper_left_x, int upper_left_y);
void display_center_mark(unsigned char image[][WIDTH][3], int center_x, int center_y);
void display_rectangle(unsigned char image[][WIDTH][3],
                       int upper_left_x, int upper_left_y,
                       int width, int height);
void ipr_load_ppm(unsigned char image[][WIDTH][3], const char path[]);
void ipr_save_ppm(unsigned char image[][WIDTH][3], const char path[]);


int main(int argc, char *argv[])
{
    unsigned char model_image[MODEL_HEIGHT][MODEL_WIDTH][3];    // q_u を計算するための画像
    unsigned char region_image[MODEL_HEIGHT][MODEL_WIDTH][3];   // p_u(y) を計算するための画像
    unsigned char src_image[HEIGHT][WIDTH][3];
    unsigned char dst_image[HEIGHT][WIDTH][3] = {{{0}}};
    double kernel[MODEL_HEIGHT][MODEL_WIDTH];
    double model_hist[NBIN];        // ヒストグラム q_u
    double candidate_hist[NBIN];    // ヒストグラム p_u(y)
    double bhatt;
    int x, y;
    double max;
    int max_x, max_y;

    ipr_load_ppm(src_image, argv[1]);
    copy_region(src_image, model_image, 359, 175);

    set_kernel(kernel);

    calc_histogram(model_hist, model_image, kernel);

    max = 0.0;
    max_x = 0;
    max_y = 0;
    for (y = 0; y < HEIGHT - MODEL_HEIGHT; y++) {
        for (x = 0; x < WIDTH - MODEL_WIDTH; x++) {

            copy_region(src_image, region_image, x, y);

            calc_histogram(candidate_hist, region_image, kernel);

            bhatt = calc_bhattacharrya(model_hist, candidate_hist);

            dst_image[y + RADIUS_HEIGHT][x + RADIUS_WIDTH][0] = 255 * bhatt;
            dst_image[y + RADIUS_HEIGHT][x + RADIUS_WIDTH][1] = 255 * bhatt;
            dst_image[y + RADIUS_HEIGHT][x + RADIUS_WIDTH][2] = 255 * bhatt;

            if (max < bhatt) {
                max = bhatt;
                max_x = x;
                max_y = y;
            }

        }
    }

    // 類似度が最大の画素を中心とする矩形を原画像に描画
    display_center_mark(src_image, max_x + RADIUS_WIDTH, max_y + RADIUS_HEIGHT);
    display_rectangle(src_image, max_x, max_y, MODEL_WIDTH, MODEL_HEIGHT);
    ipr_save_ppm(src_image, "result.ppm");

    // 類似度が最大の画素を中心とする矩形を類似度画像に描画
    display_center_mark(dst_image, max_x + RADIUS_WIDTH, max_y + RADIUS_HEIGHT);
    display_rectangle(dst_image, max_x, max_y, MODEL_WIDTH, MODEL_HEIGHT);
    ipr_save_ppm(dst_image, "similarity.pgm");

    return 0;
}


///// ここを実装 /////
void calc_histogram(double hist[], unsigned char image[][MODEL_WIDTH][3],
                    double kernel[][MODEL_WIDTH])
{
    int x, y;

    // ヒストグラムをゼロで初期化
    for (int i = 0; i < NBIN; i++){
        hist[i] = 0.0;
    }
    // 重み付きヒストグラムを算出
    for (y = 0; y < MODEL_HEIGHT; y++) {
        for (x = 0; x < MODEL_WIDTH; x++) {

            // RGB値から色番号に変換
            // image[y][x][0] 赤
            // image[y][x][1] 緑
            // image[y][x][2] 青
            // 色番号のビンに，カーネルの値をインクリメント
            int r_Q, g_Q, b_Q;
            r_Q = image[y][x][0] / Q_SIZE;
            g_Q = image[y][x][1] / Q_SIZE;
            b_Q = image[y][x][2] / Q_SIZE;
            hist[b_Q * Q_SIZE * Q_SIZE + g_Q * Q_SIZE + r_Q] += kernel[y][x];
        }
    }

    // ヒストグラムの正規化
    double sum_kernel = 0.0;
    for (y = 0; y < MODEL_HEIGHT; y++) {
        for (x = 0; x < MODEL_WIDTH; x++) {
            sum_kernel += kernel[y][x];
        }
    }
    for (int i = 0; i < NBIN; i++){
        hist[i] /= sum_kernel;
    }
}

double calc_bhattacharrya(double hist1[], double hist2[])
{
    // 正規化されたヒストグラムのバタッチャリヤ係数を計算
    double bhatt = 0.0;
    for (int i = 0; i < NBIN; i++){
        bhatt += sqrt(hist1[i] * hist2[i]);
    }
    return bhatt;
}
///// ここまで実装 /////

void set_kernel(double kernel[][MODEL_WIDTH])
{
    int x, y;
    double xx, yy;
    double tmp;

    for (y = 0; y < MODEL_HEIGHT; y++) {
        for (x = 0; x < MODEL_WIDTH; x++) {
            xx = (x - RADIUS_WIDTH)  / (double) RADIUS_WIDTH;
            yy = (y - RADIUS_HEIGHT) / (double) RADIUS_HEIGHT;
            tmp = xx * xx + yy * yy;
            kernel[y][x] = exp(-tmp);
        }
    }
}

void copy_region(unsigned char src[][WIDTH][3], unsigned char dst[][MODEL_WIDTH][3],
                 int upper_left_x, int upper_left_y)
{
    int x, y;

    for (y = 0; y < MODEL_HEIGHT; y++) {
        for (x = 0; x < MODEL_WIDTH; x++) {
            dst[y][x][0] = src[upper_left_y + y][upper_left_x + x][0];
            dst[y][x][1] = src[upper_left_y + y][upper_left_x + x][1];
            dst[y][x][2] = src[upper_left_y + y][upper_left_x + x][2];
        }
    }
}

void display_center_mark(unsigned char image[][WIDTH][3], int center_x, int center_y)
{
    int x, y;

    for (y = -1; y <= 1; y++) {
        for (x = -1; x <= 1; x++) {
            image[center_y + y][center_x + x][0] = 255;
            image[center_y + y][center_x + x][1] = 0;
            image[center_y + y][center_x + x][2] = 0;
        }
    }
}

void display_rectangle(unsigned char image[][WIDTH][3],
                       int upper_left_x, int upper_left_y,
                       int width, int height)
{
    int x, y;

    for (y = upper_left_y; y < upper_left_y + height; y++) {
        image[y][upper_left_x][0] = 255;
        image[y][upper_left_x][1] = 0;
        image[y][upper_left_x][2] = 0;

        image[y][upper_left_x + width - 1][0] = 255;
        image[y][upper_left_x + width - 1][1] = 0;
        image[y][upper_left_x + width - 1][2] = 0;
    }

    for (x = upper_left_x; x < upper_left_x + width; x++) {
        image[upper_left_y][x][0] = 255;
        image[upper_left_y][x][1] = 0;
        image[upper_left_y][x][2] = 0;

        image[upper_left_y + height - 1][x][0] = 255;
        image[upper_left_y + height - 1][x][1] = 0;
        image[upper_left_y + height - 1][x][2] = 0;
    }
}

void ipr_load_ppm(unsigned char image[][WIDTH][3], const char path[])
{
    char magic_number[2];
    int width, height;
    int max_intensity;
    FILE *fp;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "%s が開けませんでした．\n", path);
        exit(1);
    }

    fscanf(fp, "%c%c", &magic_number[0], &magic_number[1]);
    if (magic_number[0] != 'P' || magic_number[1] != '6') {
        fprintf(stderr, "%s はバイナリ型 PPM ではありません．\n", path);
        fclose(fp);
        exit(1);
    }

    fscanf(fp, "%d %d", &width, &height);
    if (width != WIDTH || height != HEIGHT) {
        fprintf(stderr, "画像のサイズが異なります．\n");
        fprintf(stderr, "  想定サイズ：WIDTH = %d, HEIGHT = %d\n", WIDTH, HEIGHT);
        fprintf(stderr, "  実サイズ：  width = %d, height = %d\n", width, height);
        fclose(fp);
        exit(1);
    }

    fscanf(fp, "%d", &max_intensity);
    if (max_intensity != 255) {
        fprintf(stderr, "最大階調値が不正な値です（%d）．\n", max_intensity);
        fclose(fp);
        exit(1);
    }

    fgetc(fp);  // 最大階調値の直後の改行コードを読み捨て

    fread(image, sizeof(unsigned char), HEIGHT * WIDTH * 3, fp);

    fclose(fp);
}

void ipr_save_ppm(unsigned char image[][WIDTH][3], const char path[])
{
    FILE *fp;

    fp = fopen(path, "wb");
    if (fp == NULL) {
        fprintf(stderr, "%s が開けませんでした．\n", path);
        exit(1);
    }

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", WIDTH, HEIGHT);
    fprintf(fp, "255\n");
    fwrite(image, sizeof(unsigned char), HEIGHT * WIDTH * 3, fp);

    fclose(fp);
}
