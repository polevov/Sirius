////////////////////////////////////////////////////////
//                                                    //
//    Библиотека функций для построения алгоритмов    //
//    автоматического раскроя листового материала     //
//                      ©2002                         //
//     Разработчик: Полевов Александр Валерьевич      //
//             аспирантура УГТУ-УПИ                   //
//       Механико Машиностроительный Факультет        //
//                 Кафедра САПР                       //
//                                                    //
////////////////////////////////////////////////////////

//#include "../stdafx.h"
#include "nclBMP.h"

CnclBMP::CnclBMP()
{
}

CnclBMP::~CnclBMP()
{

}


PBITMAPINFO CnclBMP::CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;

    // Retrieve the bitmap's color format, width, and height.
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
        return 0;
        //Err<<L"GetObject"<<ENDL;

    // Convert the color format to a count of bits.
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD
    // data structures.)

     if (cClrBits != 24)
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                    sizeof(BITMAPINFOHEADER) +
                    sizeof(RGBQUAD) * (1<< cClrBits));

     // There is no RGBQUAD array for the 24-bit-per-pixel format.

     else
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                    sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure.

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color
    // indices and store the result in biSizeImage.
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8
                                  * pbmi->bmiHeader.biHeight*cClrBits;
// Set biClrImportant to 0, indicating that all of the
    // device colors are important.
     pbmi->bmiHeader.biClrImportant = 0;
     return pbmi;
 }


void CnclBMP::CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
                  HBITMAP hBMP, HDC hDC)
{
    HANDLE hf;                 // file handle
    BITMAPFILEHEADER hdr;       // bitmap file-header
    PBITMAPINFOHEADER pbih;     // bitmap info-header
    LPBYTE lpBits;              // memory pointer
    DWORD dwTotal;              // total count of bytes
    DWORD cb;                   // incremental count of bytes
    BYTE *hp;                   // byte pointer
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER) pbi;
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits)
        return;
         //Err<<L"GlobalAlloc"<<ENDL;

    // Retrieve the color table (RGBQUAD array) and the bits
    // (array of palette indices) from the DIB.
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi,
        DIB_RGB_COLORS))
    {
        return;
        //Err<<L"GetDIBits"<<ENDL;
    }

    // Create the .BMP file.
    hf = CreateFile(pszFile,
                   GENERIC_READ | GENERIC_WRITE,
                   (DWORD) 0,
                    NULL,
                   CREATE_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL,
                   (HANDLE) NULL);
    if (hf == INVALID_HANDLE_VALUE)
        return;
        //Err<<L"CreateFile"<<ENDL;
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
    // Compute the size of the entire file.
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) +
                 pbih->biSize + pbih->biClrUsed
                 * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
                    pbih->biSize + pbih->biClrUsed
                    * sizeof (RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER),
        (LPDWORD) &dwTmp,  NULL))
    {
       return;//Err<<L"WriteFile"<<ENDL;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)
                  + pbih->biClrUsed * sizeof (RGBQUAD),
                  (LPDWORD) &dwTmp, ( NULL)) )
        return;
        //Err<<L"WriteFile"<<ENDL;

    // Copy the array of color indices into the .BMP file.
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL))
        return;
        //Err<<L"WriteFile"<<ENDL;

    // Close the .BMP file.
     if (!CloseHandle(hf))
         return;
         //Err<<L"CloseHandle"<<ENDL;

    // Free memory.
    GlobalFree((HGLOBAL)lpBits);
}


void CnclBMP::Save(CnclNest *Nest,WCHAR *FileName)
{
    WCHAR BMPFile[MAX_PATH];
    lstrcpy(BMPFile,FileName);
    lstrcpy(&BMPFile[lstrlen(BMPFile)-3],L"bmp");
    CnclRect Rect=Nest->Rect(TRUE,NULL);
    if(Rect.Width()*Rect.Height()==0)
        return;
    HDC pdcDIB;                      // контекст устройства в памяти
    HBITMAP hbmpDIB;                 // и его текущий битмап
    void *pBitsDIB=NULL;            // содержимое битмапа
    int cxDIB=640;
    int cyDIB=int(Rect.Height()/Rect.Width()*640+1);
    if(cyDIB>480)
        cyDIB=480;
    BITMAPINFOHEADER BIH;            // и заголовок
    // …
    // создаем DIB section
    // создаем структуру BITMAPINFOHEADER, описывающую наш DIB
    int iSize = sizeof(BITMAPINFOHEADER);  // размер
    memset(&BIH, 0, iSize);
    BIH.biSize = iSize;        // размер структуры
    BIH.biWidth = cxDIB;       // геометрия
    BIH.biHeight = cyDIB;      // битмапа
    BIH.biPlanes = 1;          // один план
    BIH.biBitCount = 24;       // 24 bits per pixel
    BIH.biCompression = BI_RGB;// без сжатия
    // создаем новый DC в памяти
    pdcDIB = CreateCompatibleDC(NULL);
    // создаем DIB-секцию
    hbmpDIB = CreateDIBSection(
      pdcDIB,                  // контекст устройства
      (BITMAPINFO*)&BIH,       // информация о битмапе
      DIB_RGB_COLORS,          // параметры цвета
      &pBitsDIB,               // местоположение буфера (память выделяет система)
      NULL,                    // не привязываемся к отображаемым в память файлам
      0);
    // выберем новый битмап (DIB section) для контекста устройства в памяти
    HBITMAP old_obj=(HBITMAP)SelectObject(pdcDIB, hbmpDIB);
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(pfd),1,
        PFD_SUPPORT_OPENGL|PFD_DRAW_TO_BITMAP,
        PFD_TYPE_RGBA,
        24,
        0,0,0,0,0,0,
        8,
        0,
        0,
        0,0,0,0,
        0,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0,0,0
    };
    int iPixelFormat=ChoosePixelFormat(pdcDIB,&pfd);
    SetPixelFormat(pdcDIB,iPixelFormat,&pfd);

    HDC old_hDC=wglGetCurrentDC();
    HGLRC old_hGlrc=wglGetCurrentContext();
    HGLRC g_hGlrc = wglCreateContext(pdcDIB);
    wglMakeCurrent(pdcDIB,g_hGlrc);

    glColor3d(1.0,1.0,1.0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT,vp);
    glViewport(0,0,cxDIB,cyDIB);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1,cxDIB-1,0,cyDIB+1,1,-1);
    CnclTransformView old=Application->DrawTransform;
    Application->DrawTransform.ZoomAll(cxDIB,cyDIB,Rect,TRUE);

    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    glColor3d(1,1,1);
    glVertex2i(-1,cyDIB+1);
    glVertex2i(-1,-1);
    glVertex2i(cxDIB+1,-1);
    glVertex2i(cxDIB+1,cyDIB+1);
    glEnd();
    Application->Settings.SetColor(COLOR_BMP);
    Nest->Draw(FALSE);
    Application->Settings.SetColor(COLOR_DRAW);
    glFlush();

    PBITMAPINFO pbmi=CreateBitmapInfoStruct(NULL,hbmpDIB);
    CreateBMPFile(NULL,BMPFile,pbmi,hbmpDIB,pdcDIB);

    Application->DrawTransform=old;
    glPopMatrix();
    glViewport(vp[0],vp[1],vp[2],vp[3]);

    wglMakeCurrent(old_hDC,old_hGlrc);
    wglDeleteContext(g_hGlrc);
    SelectObject(pdcDIB, NULL);
    DeleteObject(hbmpDIB);
    DeleteDC(pdcDIB);
}

void CnclBMP::Save(CnclDetail *Detail,WCHAR *FileName)
{
    WCHAR BMPFile[MAX_PATH];
    lstrcpy(BMPFile,FileName);
    lstrcpy(&BMPFile[lstrlen(BMPFile)-3],L"bmp");
    CnclRect Rect=Detail->Rect(TRUE,NULL);
    if(Rect.Width()*Rect.Height()==0)
        return;
    HDC pdcDIB;                      // контекст устройства в памяти
    HBITMAP hbmpDIB;                 // и его текущий битмап
    void *pBitsDIB=NULL;            // содержимое битмапа
    int cxDIB=640;
    int cyDIB=int(Rect.Height()/Rect.Width()*640+1);
    if(cyDIB>480)
        cyDIB=480;
    BITMAPINFOHEADER BIH;            // и заголовок
    // …
    // создаем DIB section
    // создаем структуру BITMAPINFOHEADER, описывающую наш DIB
    int iSize = sizeof(BITMAPINFOHEADER);  // размер
    memset(&BIH, 0, iSize);
    BIH.biSize = iSize;        // размер структуры
    BIH.biWidth = cxDIB;       // геометрия
    BIH.biHeight = cyDIB;      // битмапа
    BIH.biPlanes = 1;          // один план
    BIH.biBitCount = 24;       // 24 bits per pixel
    BIH.biCompression = BI_RGB;// без сжатия
    // создаем новый DC в памяти
    pdcDIB = CreateCompatibleDC(NULL);
    // создаем DIB-секцию
    hbmpDIB = CreateDIBSection(
      pdcDIB,                  // контекст устройства
      (BITMAPINFO*)&BIH,       // информация о битмапе
      DIB_RGB_COLORS,          // параметры цвета
      &pBitsDIB,               // местоположение буфера (память выделяет система)
      NULL,                    // не привязываемся к отображаемым в память файлам
      0);
    // выберем новый битмап (DIB section) для контекста устройства в памяти
    HBITMAP old_obj=(HBITMAP)SelectObject(pdcDIB, hbmpDIB);
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(pfd),1,
        PFD_SUPPORT_OPENGL|PFD_DRAW_TO_BITMAP,
        PFD_TYPE_RGBA,
        24,
        0,0,0,0,0,0,
        8,
        0,
        0,
        0,0,0,0,
        0,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0,0,0
    };
    int iPixelFormat=ChoosePixelFormat(pdcDIB,&pfd);
    SetPixelFormat(pdcDIB,iPixelFormat,&pfd);

    HDC old_hDC=wglGetCurrentDC();
    HGLRC old_hGlrc=wglGetCurrentContext();
    HGLRC g_hGlrc = wglCreateContext(pdcDIB);
    wglMakeCurrent(pdcDIB,g_hGlrc);

    glColor3d(1.0,1.0,1.0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT,vp);
    glViewport(0,0,cxDIB,cyDIB);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1,cxDIB-1,0,cyDIB+1,1,-1);
    CnclTransformView old=Application->DrawTransform;
    Application->DrawTransform.ZoomAll(cxDIB,cyDIB,Rect,TRUE);

    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    glColor3d(1,1,1);
    glVertex2i(-1,cyDIB+1);
    glVertex2i(-1,-1);
    glVertex2i(cxDIB+1,-1);
    glVertex2i(cxDIB+1,cyDIB+1);
    glEnd();
    Application->Settings.SetColor(COLOR_BMP);
    glColor4dv(Application->Settings.detail_color);
    Detail->Draw(Application->Settings.Fill,FALSE);
    Application->Settings.SetColor(COLOR_DRAW);
    glFlush();

    PBITMAPINFO pbmi=CreateBitmapInfoStruct(NULL,hbmpDIB);
    CreateBMPFile(NULL,BMPFile,pbmi,hbmpDIB,pdcDIB);

    Application->DrawTransform=old;
    glPopMatrix();
    glViewport(vp[0],vp[1],vp[2],vp[3]);

    wglMakeCurrent(old_hDC,old_hGlrc);
    wglDeleteContext(g_hGlrc);
    SelectObject(pdcDIB, NULL);
    DeleteObject(hbmpDIB);
    DeleteDC(pdcDIB);
}
