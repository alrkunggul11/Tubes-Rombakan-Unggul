#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define M_PI 3.141592653589793

// Konstanta untuk jari-jari Bumi dalam km
#define RADIUS_OF_EARTH 6371

// Struktur data untuk markas militer
typedef struct {
    char name[50];
    double latitude;
    double longitude;
} Base;

typedef struct {
    Base* items;
    int top;
    int size;
} stack;

// Mengkonversi derajat ke radian
double to_radians(double degree) {
    return degree * (M_PI / 180);
}

Base start;

void push(stack* s, Base item) {
    s->top++;
    s->items[s->top] = item;
}

// Pop an item off the stack
Base pop(stack* s) {
    Base item = s->items[s->top];
    s->top--;
    return item;
}

void swap(Base* a, Base* b) {
    Base tmp = *a;
    *a = *b;
    *b = tmp;
}

// Menghitung jarak antara dua titik dengan rumus Haversine
double haversine_distance(Base a, Base b) {
    double lat1 = to_radians(a.latitude);
    double lon1 = to_radians(a.longitude);
    double lat2 = to_radians(b.latitude);
    double lon2 = to_radians(b.longitude);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double sin_dlat = sin(dlat / 2);
    double sin_dlon = sin(dlon / 2);

    double a_val = sin_dlat * sin_dlat + cos(lat1) * cos(lat2) * sin_dlon * sin_dlon;
    double c = 2 * atan2(sqrt(a_val), sqrt(1 - a_val));

    return RADIUS_OF_EARTH * c;
}

double cross_product(Base start, Base a, Base b) {
    double crossvalue = (a.longitude - start.longitude) * (b.latitude - start.latitude) - (b.longitude - start.longitude) * (a.latitude - start.latitude);
    return crossvalue;
}

// Fungsi pembanding untuk mengurutkan markas berdasarkan sudut
int compare_angles(const void *a, const void *b) {
    Base base_a = *(Base *)a;
    Base base_b = *(Base *)b;

    double cp = cross_product(start, base_a, base_b);

    if (cp > 0) {
        return -1;
    } else if (cp < 0) {
        return 1;
    } else {
        double distance = haversine_distance(start, base_a) - haversine_distance(start, base_b);
        if (distance > 0) {
            return 1;
        } else {
            return -1;
        }
    }
}


int main() {
    // Deklarasi variabel
    char file_name[100];
    FILE *file;
    Base bases[1000], convex_hull[1000], out_of_border[1000];
    int n = 0, m = 0, o = 0;
    double total_distance = 0;

    // Meminta input file markas dari pengguna
    printf("Masukkan file markas: ");
    scanf("%s", file_name);

    // Membuka file
    file = fopen(file_name, "r");
    if (file == NULL) {
        printf("File tidak ditemukan\n");
        return 1;
    }

    // Membaca dan mengabaikan baris pertama (header)
    char buffer[256];
    fgets(buffer, 256, file); 

    // Membaca data markas dari file
    while (fgets(buffer, 256, file)) {
        sscanf(buffer, "%[^,],%lf,%lf", bases[n].name, &bases[n].latitude, &bases[n].longitude);
        n++;
    }
    // Menutup file
    fclose(file);

    // Mengurutkan markas berdasarkan sudut
    qsort(bases, n, sizeof(Base), compare_angles);
    printf(qsort);
    printf("\n");

    // Menghitung perbatasan efektif dengan algoritma Convex Hull
    for (int i = 0; i < n; i++) {
        // Menambahkan markas ke perbatasan efektif jika memenuhi syarat jarak
        while (m >= 2 && haversine_distance(convex_hull[m - 1], convex_hull[m - 2]) + haversine_distance(convex_hull[m - 2], bases[i]) <= 2500) {
            m--;
        }
        convex_hull[m++] = bases[i];
    }
    // Mengecek markas mana yang berada di luar perbatasan efektif
    for (int i = 0; i < n; i++) {
        int inside_border = 0;
        for (int j = 0; j < m; j++) {
            if (strcmp(bases[i].name, convex_hull[j].name) == 0) {
                inside_border = 1;
                break;
            }
        }
        if (!inside_border) {
            out_of_border[o++] = bases[i];
        }      
    }
    
    // Mencetak perbatasan efektif negara api
    printf("\nPerbatasan Efektif Negara Api:\n");
    for (int i = 0; i < m; i++) {
        printf("%s -> ", convex_hull[i].name);
        // Menghitung panjang perbatasan efektif
        if (i < m - 1) {
            total_distance += haversine_distance(convex_hull[i], convex_hull[i + 1]);
        }
    }
    total_distance += haversine_distance(convex_hull[m - 1], convex_hull[0]);
    printf("%s\n", convex_hull[0].name);
    printf("\nPanjang Perbatasan Efektif Negara Api: %.3lf km\n", total_distance);

    // Mencetak markas yang berada di luar perbatasan efektif
    printf("\nMarkas di Luar Perbatasan Efektif:\n");
    for (int i = 0; i < o; i++) {
        printf("%d.%s\n", i + 1, out_of_border[i].name);
    }

    return 0;
}
