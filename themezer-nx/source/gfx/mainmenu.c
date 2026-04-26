#include "gfx.h"

int lennify(Context_t *ctx){
    static int lenny = false;
    if (!lenny){
        ShapeLinkAdd(&ctx->all, ImageCreate(LeImg, POS(644, 0, 156, 60), 0), ImageType);
        lenny = true;
    }
    return 0;
}

int NextPageButton(Context_t *ctx){
    ShapeLinker_t *all = ctx->all;
    RequestInfo_t *rI = ShapeLinkFind(all, DataType)->item;

    if (rI->page >= rI->pageCount){
        return 0;
    }

    rI->page++;
    ShowLoadingPageUI(ctx, rI);

    if (MakeRequestAsCtx(ctx,rI))
        rI->page--;

    return 0;
}

int PrevPageButton(Context_t *ctx){
    ShapeLinker_t *all = ctx->all;
    RequestInfo_t *rI = ShapeLinkFind(all, DataType)->item;

    if (rI->page <= 1){
        return 0;
    }

    rI->page--;
    ShowLoadingPageUI(ctx, rI);

    if (MakeRequestAsCtx(ctx,rI))
        rI->page++;

    return 0;
}

int ButtonHandlerMainMenu(Context_t *ctx){
    if (ctx->kHeld & HidNpadButton_R)
        return NextPageButton(ctx);
    if (ctx->kHeld & HidNpadButton_L)
        return PrevPageButton(ctx);
    if (ctx->kHeld & HidNpadButton_Y)
        return ShowSideFilterMenu(ctx);
    if (ctx->kHeld & HidNpadButton_X)
        return ShowSideTargetMenu(ctx);
    if (ctx->kHeld & HidNpadButton_Minus)
        return ShowSideQueueMenu(ctx);

    return 0;
}

static SDL_Rect FitThumbHashBackground(SDL_Rect area){
    int width = area.w;
    int height = width * 9 / 16;

    if (height > area.h){
        height = area.h;
        width = height * 16 / 9;
    }

    return POS(area.x + (area.w - width) / 2, area.y + (area.h - height) / 2, width, height);
}

static SDL_Rect FitTextureCover(SDL_Texture *texture, SDL_Rect area){
    SizeInfo_t size = GetTextureSize(texture);
    if (size.w <= 0 || size.h <= 0)
        return area;

    int width = area.w;
    int height = width * size.h / size.w;

    if (height < area.h){
        height = area.h;
        width = height * size.w / size.h;
    }

    return POS(area.x + (area.w - width) / 2, area.y + (area.h - height) / 2, width, height);
}

ShapeLinker_t *CreateSplashScreen(){
    ShapeLinker_t *out = NULL;

    ShapeLinkAdd(&out, RectangleCreate(POS(0, 0, SCREEN_W, SCREEN_H), COLOR_MAINBG, 1), RectangleType);
    if (banner)
        ShapeLinkAdd(&out, ImageCreate(banner, FitTextureCover(banner, POS(0, 0, SCREEN_W, SCREEN_H)), 0), ImageType);

    return out;
}

static void AddMainMenuBackground(ShapeLinker_t **out, RequestInfo_t *rI){
    ShapeLinkAdd(out, RectangleCreate(POS(0, 0, SCREEN_W, SCREEN_H), COLOR_MAINBG, 1), RectangleType);

    if (bgTile){
        SizeInfo_t tileSize = GetTextureSize(bgTile);
        int tileW = (SCREEN_W + 2) / 3;
        int tileH = (tileSize.w > 0) ? (tileW * tileSize.h / tileSize.w) : tileW;

        if (tileH <= 0)
            tileH = tileW;

        for (int y = 0; y < SCREEN_H; y += tileH){
            for (int x = 0; x < SCREEN_W; x += tileW){
                ShapeLinkAdd(out, ImageCreate(bgTile, POS(x, y, tileW, tileH), 0), ImageType);
            }
        }
    }

    SDL_Texture *thumbHashBackground = (rI->target == 0) ? packBgThumbHash : themeBgThumbHash;
    if (thumbHashBackground){
        ShapeLinkAdd(out, ImageCreate(thumbHashBackground, FitThumbHashBackground(POS(0, 0, SCREEN_W, SCREEN_H)), 0), ImageType);
    }
}

ShapeLinker_t *CreateMainMenu(ShapeLinker_t *listItems, RequestInfo_t *rI) { 
    ShapeLinker_t *out = NULL;
    int quickIdButtonX = GetInstallButtonState() ? 360 : 240;
    SDL_Color accentColor = GetMainMenuAccentColor(rI);

    AddMainMenuBackground(&out, rI);
    ShapeLinkAdd(&out, RectangleCreate(POS(0, 0, SCREEN_W, 60), COLOR_MAIN_TOPBAR, 1), RectangleType);

    // Text inbetween arrows
    char *temp = CopyTextArgsUtil("%d/%d (%d)", rI->page, rI->pageCount, rI->itemCount);
    ShapeLinkAdd(&out, TextCenteredCreate(POS(920, 0, 240, 60), temp, COLOR_WHITE, FONT_TEXT[FSize25]), TextCenteredType);
    free(temp);

    ShapeLinkAdd(&out, TextCenteredCreate(POS(0, 60, SCREEN_W, SCREEN_H - 60), listItems ? " " : "Loading...", COLOR_WHITE, FONT_TEXT[FSize45]), TextCenteredType);
    ShapeLinkAdd(&out, ImageCreate(moodDown, POS(0, 0, 0, 0), 0), ImageType);
    ShapeLinkAdd(&out, TextCenteredCreate(POS(0, 460, SCREEN_W, 80), " ", COLOR_WHITE, FONT_TEXT[FSize35]), TextCenteredType);

    // MenuButton
    ShapeLinkAdd(&out, ButtonCreate(POS(0, 0, 120, 60), COLOR_MAIN_TOPBARBUTTONS, accentColor, COLOR_WHITE, COLOR_CURSOR, 0, ButtonStyleBottomStrip, NULL, NULL, ShowSideTargetMenu), ButtonType);
    ShapeLinkAdd(&out, ImageCreate(menuIcon, POS(30, 0, 60, 60), 0), ImageType);

    // SearchButton
    ShapeLinkAdd(&out, ButtonCreate(POS(120, 0, 120, 60), COLOR_MAIN_TOPBARBUTTONS, accentColor, COLOR_WHITE, COLOR_CURSOR, 0, ButtonStyleBottomStrip, NULL, NULL, ShowSideFilterMenu), ButtonType);
    ShapeLinkAdd(&out, ImageCreate(searchIcon, POS(150, 0, 60, 60), 0), ImageType);

    if (GetInstallButtonState()){
        // SettingsButton
        ShapeLinkAdd(&out, ButtonCreate(POS(240, 0, 120, 60), COLOR_MAIN_TOPBARBUTTONS, accentColor, COLOR_WHITE, COLOR_CURSOR, 0, ButtonStyleBottomStrip, NULL, NULL, ShowSideQueueMenu), ButtonType);
        ShapeLinkAdd(&out, ImageCreate(queueIcon, POS(270, 0, 60, 60), 0), ImageType);
    }

    ShapeLinkAdd(&out, ButtonCreate(POS(quickIdButtonX, 0, 120, 60), COLOR_MAIN_TOPBARBUTTONS, accentColor, COLOR_WHITE, COLOR_CURSOR, 0, ButtonStyleBottomStrip, NULL, NULL, ShowQuickIdLookup), ButtonType);
    ShapeLinkAdd(&out, TextCenteredCreate(POS(quickIdButtonX, 0, 120, 60), "ID", COLOR_WHITE, FONT_TEXT[FSize30]), TextCenteredType);
    ShapeLinkAdd(&out, TextCenteredCreate(POS(quickIdButtonX + 1, 0, 120, 60), "ID", COLOR_WHITE, FONT_TEXT[FSize30]), TextCenteredType);

    // LeftArrow
    ShapeLinkAdd(&out, ButtonCreate(POS(800, 0, 120, 60), COLOR_MAIN_TOPBARBUTTONS, rI->page > 1 ? accentColor : COLOR_MAIN_TOPBARBUTTONS, COLOR_WHITE, COLOR_CURSOR, 0, ButtonStyleBottomStrip, NULL, NULL, PrevPageButton), ButtonType);
    ShapeLinkAdd(&out, ImageCreate(arrowLIcon, POS(830, 0, 60, 60), 0), ImageType);

    ShapeLinkAdd(&out, ButtonCreate(POS(644, 0, 156, 60), COLOR_MAIN_TOPBARBUTTONS, COLOR_MAIN_TOPBARBUTTONS, COLOR_WHITE, COLOR_MAIN_TOPBARBUTTONS, BUTTON_NOJOYSEL, ButtonStyleFlat, NULL, NULL, lennify), ButtonType);

    // RightArrow
    ShapeLinkAdd(&out, ButtonCreate(POS(1160, 0, 120, 60), COLOR_MAIN_TOPBARBUTTONS, rI->page < rI->pageCount ? accentColor : COLOR_MAIN_TOPBARBUTTONS, COLOR_WHITE, COLOR_CURSOR, 0, ButtonStyleBottomStrip, NULL, NULL, NextPageButton), ButtonType);
    ShapeLinkAdd(&out, ImageCreate(arrowRIcon, POS(1190, 0, 60, 60), 0), ImageType);

    ShapeLinkAdd(&out, ListGridCreate(POS(0, 60, SCREEN_W, SCREEN_H - 60), 4, 260, COLOR_FROM_RGBA(COLOR_TRANSPARENT_RGBA), accentColor, accentColor, COLOR_SCROLLBARBG, accentColor, (listItems) ? GRID_NOSIDEESC : LIST_DISABLED, listItems, ThemeSelect, NULL, FONT_TEXT[FSize23]), ListGridType);
    // 4, 260

    ShapeLinkAdd(&out, rI, DataType);

    // Logo
    ShapeLinkAdd(&out, ImageCreate(icon, POS(584, 0, 60, 60), 0), ImageType);

    // Glyphs
    ShapeLinkAdd(&out, GlyphCreate(97, 2, BUTTON_X, COLOR_WHITE, FONT_BTN[FSize20]), GlyphType);
    ShapeLinkAdd(&out, GlyphCreate(217, 2, BUTTON_Y, COLOR_WHITE, FONT_BTN[FSize20]), GlyphType);
    ShapeLinkAdd(&out, GlyphCreate(337, 2, BUTTON_MINUS, COLOR_WHITE, FONT_BTN[FSize20]), GlyphType);

    Glyph_t *leftButtonIcon = GlyphCreate(804, 2, BUTTON_L, COLOR_WHITE, FONT_BTN[FSize20]);
    Glyph_t *rightButtonIcon = GlyphCreate(1256, 2, BUTTON_R, COLOR_WHITE, FONT_BTN[FSize20]);

    if (rI->page == 1) {
        SETBIT(leftButtonIcon->options, TEXT_GLYPH_NO_RENDER, 1);
    }
    if (rI->page == rI->pageCount) {
        SETBIT(rightButtonIcon->options, TEXT_GLYPH_NO_RENDER, 1);
    }

    ShapeLinkAdd(&out, leftButtonIcon, GlyphType);
    ShapeLinkAdd(&out, rightButtonIcon, GlyphType);

    return out;
}