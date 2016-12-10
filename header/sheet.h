/* sheet.c */
#define MAX_SHEETS		256
typedef struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
}Sheet, *SheetPtr;
typedef struct SHTCTL {
	unsigned char *vram;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
}ShtCtl, *ShtCtlPtr;
void ShtCtl__construct(struct SHTCTL *ctl, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void Sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void Sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height);
void Sheet_refresh(struct SHTCTL *ctl);
void Sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0);
void Sheet_free(struct SHTCTL *ctl, struct SHEET *sht);