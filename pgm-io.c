#include <stdio.h>

#define HEIGHT 256
#define WIDTH 256

int main()
{
    char magic_number[2];
    int width, height;
    int max_intensity;
    unsigned char src_img[HEIGHT][WIDTH];
    unsigned char dst_img[HEIGHT][WIDTH];
    FILE *fp;
    int m,n;

    fp = fopen("girl.pgm", "rb");
    fscanf(fp, "%c%c", &magic_number[0], &magic_number[1]);
    fscanf(fp, "%d %d", &width, &height);
    fscanf(fp, "%d", &max_intensity);
    fgetc(fp);

    fread(src_img, sizeof(unsigned char), HEIGHT * WIDTH, fp);  /* 画像データの読み込み */
    fclose(fp);

    for (m = 0; m < HEIGHT; m++) {
        for (n = 0; n < WIDTH; n++) {
            dst_img[m][n] = src_img[m][n];
        }
    }
    fp = fopen("girl1.pgm", "wb");                              /* 書き出し画像ファイルのオープン */
    fprintf(fp, "%c%c\n", magic_number[0], magic_number[1]);    /* ヘッダの書き出し */
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "%d\n", max_intensity);
    fwrite(dst_img, sizeof(unsigned char), HEIGHT * WIDTH, fp); /* 画像データの書き出し */
    fclose(fp);                                                 /* 書き出し画像のクローズ */
 
    return 0;

}