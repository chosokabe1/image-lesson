#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 576
#define WIDTH  768
#define IPR_GRAD_H 0
#define IPR_GRAD_V 1

typedef unsigned char uchar;
void ipr_sobel(double sobel[][WIDTH], unsigned char image[][WIDTH], int direction);
void sobel_horizontal(uchar image[][WIDTH], double sobel[][WIDTH]);
void sobel_vertical(uchar image[][WIDTH], double sobel[][WIDTH]);
void time_derivative(double derivative[][WIDTH], uchar image_t[][WIDTH], uchar image_pre[][WIDTH]);
void sobel_to_ppm(double sobel[][WIDTH], uchar image[][WIDTH][3]);
void ipr_load_pgm(uchar image[][WIDTH], const char path[]);
void ipr_save_ppm(uchar image[][WIDTH][3], const char path[]);

int main(int argc, char *argv[])
{
    // uchar src_image[HEIGHT][WIDTH];
    uchar dst_image[HEIGHT][WIDTH][3];
    double sobelX[HEIGHT][WIDTH];
    double sobelY[HEIGHT][WIDTH];
    double derivative[HEIGHT][WIDTH];
    double flow[HEIGHT][WIDTH][2];

    // ipr_load_pgm(src_image, argv[1]);
    // ipr_sobel(sobel, src_image, IPR_GRAD_H);
    // sobel_to_ppm(sobel, dst_image);
    // ipr_save_ppm(dst_image, argv[2]);

    // ipr_sobel(sobel, src_image, IPR_GRAD_V);
    // sobel_to_ppm(sobel, dst_image);
    // ipr_save_ppm(dst_image, argv[3]);

    uchar image_t[HEIGHT][WIDTH];
    uchar image_pre[HEIGHT][WIDTH];
    ipr_load_pgm(image_t, argv[1]);
    ipr_load_pgm(image_pre, argv[2]);
    ipr_sobel(sobelX, image_t, IPR_GRAD_H);
    ipr_sobel(sobelY, image_t, IPR_GRAD_V);
    time_derivative(derivative, image_t, image_pre);

    ipr_save_ppm(dst_image, argv[3]);

    return 0;
}
void flow(double flow[HEIGHT][WIDTH][2], double sobelX[][WIDTH], double sobelY[][WIDTH], double derivative[][WIDTH]){
    
}
void time_derivative(double derivative[][WIDTH], uchar image_t[][WIDTH], uchar image_pre[][WIDTH]){
    /////
    // 四隅の微分値の計算
    /////
    
    // (0, 0) の計算
    derivative[0][0]
        = 2 * image_t[0][0] + image_t[0][0] + image_t[0][0] + image_t[1][0] + image_t[0][1] - 2 * image_pre[0][0] - image_pre[0][0] - image_pre[0][0] - image_pre[1][0] - image_pre[0][1];
    
    // (0, W - 1) の計算
    derivative[0][WIDTH - 1]
        = 2 * image_t[0][WIDTH - 1] + image_t[0][WIDTH - 1] + image_t[0][WIDTH - 1] + image_t[1][WIDTH - 1] + image_t[0][WIDTH - 2] - 2 * image_pre[0][WIDTH - 1] - image_pre[0][WIDTH - 1] - image_pre[0][WIDTH - 1] - image_pre[1][WIDTH - 1] - image_pre[0][WIDTH - 2];
    
    // (H - 1, 0) の計算
    derivative[HEIGHT - 1][0]
        = 2 * image_t[HEIGHT - 1][0] + image_t[HEIGHT - 1][0] + image_t[HEIGHT - 1][0] + image_t[HEIGHT - 2][0] + image_t[HEIGHT - 1][1] - 2 * image_pre[HEIGHT - 1][0] - image_pre[HEIGHT - 1][0] - image_pre[HEIGHT - 1][0] - image_pre[HEIGHT - 2][0] - image_pre[HEIGHT - 1][1];

    // (H - 1, W - 1) の計算
    derivative[HEIGHT - 1][WIDTH - 1]
        = 2 * image_t[HEIGHT - 1][WIDTH - 1] + image_t[HEIGHT - 1][WIDTH - 1] + image_t[HEIGHT - 1][WIDTH - 1] + image_t[HEIGHT - 2][WIDTH - 1] + image_t[HEIGHT - 1][WIDTH - 2] - 2 * image_pre[HEIGHT - 1][WIDTH - 1] - image_pre[HEIGHT - 1][WIDTH - 1] - image_pre[HEIGHT - 1][WIDTH - 1] - image_pre[HEIGHT - 2][WIDTH - 1] - image_pre[HEIGHT - 1][WIDTH - 2];

    /////
    // 四隅を除く端の微分値の計算
    /////
    
    // 上端である (0    , 1) -- (0    , W - 2) と
    // 下端である (H - 1, 1) -- (H - 1, W - 2) の計算
    for (int x = 1; x < WIDTH - 2; x++) {
        derivative[0][x]
            = 2 * image_t[0][x] + image_t[0][x] + image_t[0][x - 1] + image_t[1][x] + image_t[0][x + 1]
            - 2 * image_pre[0][x] - image_pre[0][x] - image_pre[0][x - 1] - image_pre[1][x] - image_pre[0][x + 1];
        
        derivative[HEIGHT - 1][x]
            = 2 * image_t[HEIGHT - 1][x] + image_t[HEIGHT - 2][x] + image_t[HEIGHT - 1][x - 1] + image_t[HEIGHT - 1][x] + image_t[HEIGHT - 1][x + 1]
            - 2 * image_pre[HEIGHT - 1][x] - image_pre[HEIGHT - 2][x] - image_pre[HEIGHT - 1][x - 1] - image_pre[HEIGHT - 1][x] - image_pre[HEIGHT - 1][x + 1];
    }

    // 左端である (1, 0)     -- (H - 2, 0)     と
    // 右端である (1, W - 1) -- (H - 2, W - 1) の計算
    for (int y = 1; y < HEIGHT - 2; y++) {
        derivative[y][0]
            = 2 * image_t[y][0] + image_t[y - 1][0] + image_t[y][0] + image_t[y + 1][0] + image_t[y][1]
            - 2 * image_pre[y][0] - image_pre[y - 1][0] - image_pre[y][0] - image_pre[y + 1][0] - image_pre[y][1];
        
        derivative[y][WIDTH - 1]
            = 2 * image_t[y][WIDTH - 1] + image_t[y - 1][WIDTH - 1] + image_t[y][WIDTH - 1] + image_t[y + 1][WIDTH - 1] + image_t[y][WIDTH - 2]
            - 2 * image_pre[y][WIDTH - 1] - image_pre[y - 1][WIDTH - 1] - image_pre[y][WIDTH - 1] - image_pre[y + 1][WIDTH - 1] - image_pre[y][WIDTH - 2];
    }

    /////
    // 端を除く領域の計算
    /////
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            derivative[y][x]
                = 2 * image_t[y][x] + image_t[y - 1][x] + image_t[y][x-1] + image_t[y + 1][x] + image_t[y][x+1]
                - 2 * image_pre[y][x] - image_pre[y - 1][x] - image_pre[y][x-1] - image_pre[y + 1][x] - image_pre[y][x+1];
        }
    }

    /////
    // 正規化のために 1/6 倍する
    /////
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            derivative[y][x] /= 6;
        }
    }
}

void ipr_sobel(double sobel[][WIDTH], unsigned char image[][WIDTH], int direction){
    if( direction == IPR_GRAD_H){
        sobel_horizontal(image, sobel);
    } else if (direction == IPR_GRAD_V){
        sobel_vertical(image, sobel);
    }
}
void sobel_horizontal(uchar image[][WIDTH], double sobel[][WIDTH])
{
    /////
    // 四隅の微分値の計算
    /////
    
    // (0, 0) の計算
    sobel[0][0]
        = image[0][1] + 2 * image[0][1] + image[1][1]
        - image[0][0] - 2 * image[0][0] - image[1][0];
    
    // (0, W - 1) の計算
    sobel[0][WIDTH - 1]
        = image[0][WIDTH - 1] + 2 * image[0][WIDTH - 1] + image[1][WIDTH - 1]
        - image[0][WIDTH - 2] - 2 * image[0][WIDTH - 2] - image[1][WIDTH - 2];
    
    // (H - 1, 0) の計算
    sobel[HEIGHT - 1][0]
        = image[HEIGHT - 2][1] + 2 * image[HEIGHT - 1][1] + image[HEIGHT - 1][1]
        - image[HEIGHT - 2][0] - 2 * image[HEIGHT - 1][0] - image[HEIGHT - 1][0];
    
    // (H - 1, W - 1) の計算
    sobel[HEIGHT - 1][WIDTH - 1]
        = image[HEIGHT - 2][WIDTH - 1] + 2 * image[HEIGHT - 1][WIDTH - 1]
        + image[HEIGHT - 1][WIDTH - 1]
        - image[HEIGHT - 2][WIDTH - 2] - 2 * image[HEIGHT - 1][WIDTH - 2]
        - image[HEIGHT - 1][WIDTH - 2];
    
    /////
    // 四隅を除く端の微分値の計算
    /////
    
    // 上端である (0    , 1) -- (0    , W - 2) と
    // 下端である (H - 1, 1) -- (H - 1, W - 2) の計算
    for (int x = 1; x < WIDTH - 2; x++) {
        sobel[0][x]
            = image[0][x + 1] + 2 * image[0][x + 1] + image[1][x + 1]
            - image[0][x - 1] - 2 * image[0][x - 1] - image[1][x - 1];
        
        sobel[HEIGHT - 1][x]
            = image[HEIGHT - 2][x + 1] + 2 * image[HEIGHT - 1][x + 1] + image[HEIGHT - 1][x + 1]
            - image[HEIGHT - 2][x - 1] - 2 * image[HEIGHT - 1][x - 1] - image[HEIGHT - 1][x - 1];
    }

    // 左端である (1, 0)     -- (H - 2, 0)     と
    // 右端である (1, W - 1) -- (H - 2, W - 1) の計算
    for (int y = 1; y < HEIGHT - 2; y++) {
        sobel[y][0]
            = image[y - 1][1] + 2 * image[y][1] + image[y + 1][1]
            - image[y - 1][0] - 2 * image[y][0] - image[y + 1][0];
        
        sobel[y][WIDTH - 1]
            = image[y - 1][WIDTH - 1] + 2 * image[y][WIDTH - 1] + image[y + 1][WIDTH - 1]
            - image[y - 1][WIDTH - 2] - 2 * image[y][WIDTH - 2] - image[y + 1][WIDTH - 2];
    }
    
    /////
    // 端を除く領域の計算
    /////
    
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            sobel[y][x]
                = image[y - 1][x + 1] + 2 * image[y][x + 1] + image[y + 1][x + 1]
                - image[y - 1][x - 1] - 2 * image[y][x - 1] - image[y + 1][x - 1];
        }
    }

    /////
    // 正規化のために 0.125 倍する
    /////
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            sobel[y][x] *= 0.125;
        }
    }
}
void sobel_vertical(uchar image[][WIDTH], double sobel[][WIDTH])
{
    /////
    // 四隅の微分値の計算
    /////

    // (0, 0) の計算
    sobel[0][0]
        = image[1][0] + 2 * image[1][0] + image[1][1]
        - image[0][0] - 2 * image[0][0] - image[0][1];
    
    // (0, W - 1) の計算
    sobel[0][WIDTH - 1]
        = image[1][WIDTH - 2] + 2 * image[1][WIDTH - 1] + image[1][WIDTH - 1]
        - image[0][WIDTH - 2] - 2 * image[0][WIDTH - 1] - image[0][WIDTH - 1];
    
    // (H - 1, 0) の計算
    sobel[HEIGHT - 1][0]
        = image[HEIGHT - 1][0] + 2 * image[HEIGHT - 1][0] + image[HEIGHT - 1][1]
        - image[HEIGHT - 2][0] - 2 * image[HEIGHT - 2][0] - image[HEIGHT - 2][1];
    
    // (H - 1, W - 1) の計算
    sobel[HEIGHT - 1][WIDTH - 1]
        = image[HEIGHT - 1][WIDTH - 2] + 2 * image[HEIGHT - 1][WIDTH - 1]
        + image[HEIGHT - 1][WIDTH - 1]
        - image[HEIGHT - 2][WIDTH - 2] - 2 * image[HEIGHT - 2][WIDTH - 1]
        - image[HEIGHT - 2][WIDTH - 1];

    /////
    // 四隅を除く端の微分値の計算
    /////
    
    // 上端である (0    , 1) -- (0    , W - 2) と
    // 下端である (H - 1, 1) -- (H - 1, W - 2) の計算
    for (int x = 1; x < WIDTH - 2; x++) {
        sobel[0][x]
            = image[1][x - 1] + 2 * image[1][x] + image[1][x + 1]
            - image[0][x - 1] - 2 * image[0][x] - image[0][x + 1];
        
        sobel[HEIGHT - 1][x]
            = image[HEIGHT - 1][x + 1] + 2 * image[HEIGHT - 1][x + 1] + image[HEIGHT - 1][x + 1]
            - image[HEIGHT - 2][x - 1] - 2 * image[HEIGHT - 2][x] - image[HEIGHT - 2][x + 1];
    }
    
    // 左端である (1, 0)     -- (H - 2, 0)     と
    // 右端である (1, W - 1) -- (H - 2, W - 1) の計算
    for (int y = 1; y < HEIGHT - 2; y++) {
        sobel[y][0]
            = image[y + 1][0] + 2 * image[y + 1][0] + image[y + 1][1]
            - image[y - 1][0] - 2 * image[y - 1][0] - image[y - 1][1];
        
        sobel[y][WIDTH - 1]
            = image[y + 1][WIDTH - 2] + 2 * image[y + 1][WIDTH - 1] + image[y + 1][WIDTH - 1]
            - image[y - 1][WIDTH - 2] - 2 * image[y - 1][WIDTH - 1] - image[y - 1][WIDTH - 1];
    }

    /////
    // 端を除く領域の計算
    /////
    
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            sobel[y][x]
                = image[y + 1][x - 1] + 2 * image[y + 1][x] + image[y + 1][x + 1]
                - image[y - 1][x - 1] - 2 * image[y - 1][x] - image[y - 1][x + 1];
        }
    }

    /////
    // 正規化のために 0.125 倍する
    /////
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            sobel[y][x] *= 0.125;
        }
    }
}
void sobel_to_ppm(double sobel[][WIDTH], uchar image[][WIDTH][3]){
    double posi_max = 0;
    double nega_max = 0;

    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            if(posi_max < sobel[y][x]){
                posi_max = sobel[y][x];
            }
            if(nega_max > sobel[y][x]){
                nega_max = sobel[y][x];
            }
        }
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            if(sobel[y][x] < 0){
                image[y][x][0] = (uchar)(-1 * 255 * sobel[y][x] / nega_max + 255);
                image[y][x][1] = (uchar)(-1 * 255 * sobel[y][x] / (double)nega_max + 255);
                image[y][x][2] = 255;
            } else {
                image[y][x][0] = 255;
                image[y][x][1] = (uchar)(-1 * 255 * sobel[y][x] / (double)posi_max + 255);
                image[y][x][2] = (uchar)(-1 * 255 * sobel[y][x] / (double)posi_max + 255);
            }
        }
    }
}
void ipr_load_pgm(uchar image[][WIDTH], const char path[])
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
    if (magic_number[0] != 'P' || magic_number[1] != '5') {
        fprintf(stderr, "%s はバイナリ型 PGM ではありません．\n", path);
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

    fread(image, sizeof(uchar), HEIGHT * WIDTH, fp);
    
    fclose(fp);
}
void ipr_save_ppm(uchar image[][WIDTH][3], const char path[])
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
    fwrite(image, sizeof(uchar), HEIGHT * WIDTH * 3, fp);
    
    fclose(fp);
}