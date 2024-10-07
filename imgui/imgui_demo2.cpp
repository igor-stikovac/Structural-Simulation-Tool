#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"

#ifndef IMGUI_DISABLE

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "globalne.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <ctype.h>          
#include <cmath>
#include <limits.h>         
#include <math.h>           
#include <stdio.h>          
#include <stdlib.h>       
#include <stdint.h>        
#include <string>
#if !defined(_MSC_VER) || _MSC_VER >= 1800
#include <inttypes.h>       
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4127)     
#pragma warning (disable: 4996)     
#pragma warning (disable: 26451)   
#endif

#if defined(__clang__)
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                
#pragma clang diagnostic ignored "-Wold-style-cast"                 
#pragma clang diagnostic ignored "-Wdeprecated-declarations"        
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"       
#pragma clang diagnostic ignored "-Wformat-security"                
#pragma clang diagnostic ignored "-Wexit-time-destructors"          
#pragma clang diagnostic ignored "-Wunused-macros"                  
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  
#pragma clang diagnostic ignored "-Wdouble-promotion"               
#pragma clang diagnostic ignored "-Wreserved-id-macro"              
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                  
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"     
#pragma GCC diagnostic ignored "-Wformat-security"          
#pragma GCC diagnostic ignored "-Wdouble-promotion"         
#pragma GCC diagnostic ignored "-Wconversion"               
#pragma GCC diagnostic ignored "-Wmisleading-indentation"   
#endif

#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#else
#define IM_NEWLINE  "\n"
#endif

#if defined(_MSC_VER) && !defined(snprintf)
#define snprintf    _snprintf
#endif
#if defined(_MSC_VER) && !defined(vsnprintf)
#define vsnprintf   _vsnprintf
#endif

#if !defined(PRId64) && defined(_MSC_VER)
#define PRId64 "I64d"
#define PRIu64 "I64u"
#elif !defined(PRId64)
#define PRId64 "lld"
#define PRIu64 "llu"
#endif

#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

#ifndef IMGUI_CDECL
#ifdef _MSC_VER
#define IMGUI_CDECL __cdecl
#else
#define IMGUI_CDECL
#endif
#endif
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

ImDrawList* draw_list;
ImVector<POslonac> pokretni;
ImVector<NOslonac> nepokretni;
ImVector<Sila> sile;
ImVector<Stap> stapovi;
ImVector<Moment> momenti;
static ImVector<ImVec2> points;
ImVector<int> cvorovi;

ImGuiWindowFlags window_flags = 0;

using namespace std;


static void ShowExampleAppCustomRendering(bool* p_open);
static void prikaziLeviProzor(bool* p_open);

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback      GImGuiDemoMarkerCallback;
extern void*                        GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback             GImGuiDemoMarkerCallback = NULL;
void*                               GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)


ofstream ispis;
//---------------------------------

int brojKarakterisitka()
{
    int broj = 1;
 
    for ( int i = 1; i < brLinija; i++ ) 
    {
        int j = 0;
        for ( j = 0; j < i; j++ )
            if ( stapovi[i].E == stapovi[j].E && stapovi[i].A == stapovi[j].A && stapovi[i].I == stapovi[j].I )
                break;

        if ( i == j )
            broj++;
    }
    return broj;
}

int brojCvorova()
{
    int broj = 1;
 
    for ( int i = 1; i < points.Size; i++ ) 
    {
        int j = 0;
        for ( j = 0; j < i; j++ )
            if ( points[i].x == points[j].x && points[i].y == points[j].y )
                break;

        if ( i == j )
            broj++;
    }
    return broj;
}

void stampajCvorove()
{
    int br = 1;

    if( points.Size ) 
    {
        ispis << "0:\t" << points[0].x / GRID_STEP << "\t" << points[0].y / GRID_STEP << endl;
        cvorovi.push_back(brCvorova);
        brCvorova++;
    }
    for ( int i = 1; i < points.Size; i++ ) 
    {
        int j = 0;
        for ( j = 0; j < i; j++ )
            if ( points[i].x == points[j].x && points[i].y == points[j].y )
            {
                int pom = cvorovi[j];
                cvorovi.push_back(pom);
                
                break;
            }
                

        if ( i == j )
        {
            ispis << br << ":\t" << points[i].x / GRID_STEP << "\t" << points[i].y / GRID_STEP << endl;
            cvorovi.push_back(brCvorova);
            brCvorova++;
            br++;
        }
            
    }

}

void odrediSile()
{
    for( int i = 0 ; i < points.Size; i++ )
    {
        for( int j = 0 ; j < sile.Size; j++ )
        {
            if( sile[j].poc.x - 345 == points[i].x && sile[j].poc.y - 80 == points[i].y )
            {
                sile[j].cvor = cvorovi[i];
            }
        }
    }
}

void odrediMomente()
{
    for( int i = 0 ; i < momenti.Size; i++ )
    {
        for( int j = 0 ; j < momenti.Size; j++ )
        {
            if( momenti[j].centar.x - 350 == points[i].x && momenti[j].centar.y - 80 == points[i].y )
            {
                momenti[j].cvor = cvorovi[i];
            }
        }
    }
}

void odrediteOslonce()
{
    bool ind1 = 1, ind2 = 1;
    int cvorPre = -1;

    for( int i = 0 ; i < pokretni.Size || i < nepokretni.Size; i++ )
    {
        if( i > pokretni.Size   ) ind1 = 0;
        if( i > nepokretni.Size ) ind2 = 0;

        for( int j = 0 ; j < points.Size; j++ )
        {
            if( ind1 && cvorovi[j] > cvorPre && ( pokretni[i].t0.x - 345 == points[j].x && pokretni[i].t0.y - 80 == points[j].y ) )
            {
                pokretni[i].cvor = cvorovi[j];
                ispis << pokretni[i].cvor << "\t" << pokretni[i].pomeranjeX << "\t\t" << pokretni[i].pomeranjeY << endl;
            }

            if( ind2 && cvorovi[j] > cvorPre && ( nepokretni[i].t0.x - 345 == points[j].x && nepokretni[i].t0.y - 80 == points[j].y ) )
            {
                nepokretni[i].cvor = cvorovi[j];
                ispis << nepokretni[i].cvor << "\t" << "0" << "\t\t" << "0" << endl;
            }
            cvorPre = cvorovi[j]; 
        }
        
    }
}


void prikazPodataka() 
{
	int i;
    int brCv = brojCvorova();
	ispis.open("../izlaz/MKE-2D-ulz.txt");

	ispis<<"\n === Izlazni podaci ===\n"<<endl;

	ispis << "\nBroj cvorova : " << brCv << endl;
	ispis << "\nBroj stapova : " << brLinija <<endl;

    ispis << "\nBroj karakteristika : " << brojKarakterisitka() << endl;

	ispis << "\nBroj opterecenih cvorova : " << sile.Size << endl;

	ispis << "\nBroj sprecenih pomeranja : " << pokretni.Size + nepokretni.Size * 2 << endl;

	ispis << "\n\n\n\nKoordinate cvorova" << endl;
	ispis << "\nCvor \tx \ty\n" << endl;

	stampajCvorove();
	
	ispis<<"\n\n\n\nPrikaz definicije stapova"<<endl;

	ispis<<"\nStap   Cvor   Cvor \tOsobine\n"<<endl;

	for( i = 0; i < brLinija; i++ )
    {
		ispis << i << "\t" << cvorovi[2*i] << "\t" << cvorovi[2*i+1] << "\t" << stapovi[i].osobina << endl;
	}

	ispis<<"\n\n\n\nPrikaz karakteristika stapova"<<endl;

	ispis<<"\n Stap \tE\t A\tI\n"<<endl;

	for( i = 0; i < brLinija; i++ ) 
    {
		ispis << stapovi[i].brStapa << "\t" << stapovi[i].E << "\t" << stapovi[i].A << "\t" << stapovi[i].I << endl;
	}

	ispis << "\n\n\n\nPrikaz opterecenja po cvorovima\n" << endl;

    odrediSile();
    odrediMomente();

    int prethodniCvor = -1;

	for( i = 0; i < cvorovi.Size; i++ )
    {
        for( int j = 0 ; j < sile.Size && cvorovi[i] > prethodniCvor; j++ )
        {
            if( cvorovi[i] == sile[j].cvor )
            {
                ispis << sile[j].cvor << "\t" << sile[j].intenzitetX << "\t" << sile[j].intenzitetY << endl;
            }
        }

        prethodniCvor = cvorovi[i];
		
	}

    ispis << "\n Cvor \t intenzitet-M \t smer-M\n" << endl;

    prethodniCvor = -1;

    for( i = 0; i < cvorovi.Size; i++ )
    {
        for( int j = 0 ; j < momenti.Size && cvorovi[i] > prethodniCvor; j++ )
        {
            if( j < momenti.Size && cvorovi[i] == momenti[j].cvor )
            {
                int smer = !(momenti[j].smer) ? -1 : 1;
                ispis << momenti[j].cvor << "\t" << momenti[j].intenzitet << "\t\t" << smer << endl;
            }
        }

        prethodniCvor = cvorovi[i];
	}



	ispis<<"\n\n\n\nPrikaz sprecenih pomeranja"<<endl;

	ispis<<"\nCvor Pomeranje-X Pomeranje-Y\n\n"<<endl;

    odrediteOslonce();
 
}

void nacrtaj_moment( int x, int y )
{
    Moment pom;
    
    pom.centar.x = x + 350;
    pom.centar.y = y + 80;
    
    if( smerMomenta == 1 )
    {
        ImVec2 p1( x + 350, y - 20 + 80 );
        ImVec2 p2( x + 350, y - 10 + 80 );
        ImVec2 p3( x + 350 - 10, y - 15 + 80 );

        pom.t1 = p1;
        pom.t2 = p2;
        pom.t3 = p3;
    }
    else
    {
        ImVec2 p1( x + 350, y + 80 + 20 );
        ImVec2 p2( x + 350, y + 80 + 10 );
        ImVec2 p3( x + 350 - 10, y + 80 + 15 );

        pom.t1 = p1;
        pom.t2 = p2;
        pom.t3 = p3;
    }

    pom.smer = smerMomenta;
    pom.intenzitet = intenzitetMomenta;

    momenti.push_back(pom);
}

void strelica( int x, int y, float angle ) 
{
    // Konverzija ugla iz stepeni u radijane
    float length = 50;
    float arrowSize = 10.0f;
    Sila pom;
    float angleRad = angle * M_PI / 180.0f;

    ImVec2 start = ImVec2( x + 345, y + 80 );
    // Računanje krajnje tačke strelice
    ImVec2 end = ImVec2(start.x + length * cos(angleRad), start.y + length * sin(angleRad));
    pom.poc = start;
    pom.kraj = end;
    pom.ugaoR = -angleRad;
    pom.ugaoS = -angle;
    
    // Računanje vektora pravca strelice
    ImVec2 dir = ImVec2(end.x - start.x, end.y - start.y);
    float norm = sqrtf(dir.x * dir.x + dir.y * dir.y);

    // Normalizacija vektora
    dir.x /= norm;
    dir.y /= norm;

    // Računanje normala na vektor (rotacija za 90 stepeni)
    ImVec2 normal1 = ImVec2(-dir.y, dir.x);
    ImVec2 normal2 = ImVec2(dir.y, -dir.x);

    // Računanje tačaka za trouglove
    ImVec2 p1 = ImVec2(end.x - arrowSize * dir.x + arrowSize * normal1.x,
                       end.y - arrowSize * dir.y + arrowSize * normal1.y);
    ImVec2 p2 = ImVec2(end.x - arrowSize * dir.x + arrowSize * normal2.x,
                       end.y - arrowSize * dir.y + arrowSize * normal2.y);

    // Crtanje trouglova
    ImVec2 t0( end.x, end.y );
    ImVec2 t1( p1.x, p1.y );
    ImVec2 t2( p2.x, p2.y );

    pom.t0 = t0;
    pom.t1 = t1;
    pom.t2 = t2;
    if( -angle == 0 || -angle == 180  )
    {
        pom.intenzitetX = intenzitetSile;
        pom.intenzitetY = 0.0;
    }
    else if( -angle == 90 || -angle == 270 )
    {
        pom.intenzitetY = intenzitetSile;
        pom.intenzitetX = 0.0;
    }
    else
    {
        pom.intenzitetX = intenzitetSile * cos( -angleRad );
        pom.intenzitetY = intenzitetSile * sin( -angleRad );
    }

    sile.push_back(pom);

    sila = 0;
}

void napuni_trougao( int vrhx, int vrhy, int x1, int y1, int x2, int y2 )
{   
    ImVec2 t0(vrhx+345,vrhy+80);
    ImVec2 t1(x1+345,y1+80);
    ImVec2 t2(x2+345,y2+80);

    NOslonac pom;
    pom.t0 = t0;
    pom.t1 = t1;
    pom.t2 = t2;

    nepokretni.push_back(pom);
    nepokretan = 0;

};

void napuni_trougao_p( int vrhx, int vrhy, int x1, int y1, int x2, int y2, int a, int b, int c, int d, bool indX, bool indY )
{
    ImVec2 t0(vrhx+345,vrhy+80);
    ImVec2 t1(x1+345,y1+80);
    ImVec2 t2(x2+345,y2+80);
    ImVec2 poc(vrhx+a+345,vrhy+b+80);
    ImVec2 kraj(vrhx+c+345,vrhy+d+80);

    POslonac pom;
    pom.t0 = t0;
    pom.t1 = t1;
    pom.t2 = t2;
    pom.poc = poc;
    pom.kraj = kraj;
    pom.pomeranjeX = indX;
    pom.pomeranjeY = indY;


    pokretni.push_back(pom);
    pokretan = 0;

};


void napuni_pokretni( int vrhx, int vrhy, int ugao )
{
    if( ugao == 0   ) 
    {
        napuni_trougao_p( vrhx, vrhy, vrhx + 10.0, vrhy - 10.0, vrhx + 10.0, vrhy + 10.0, 15, -10, 15, 10, 0, 1 );  
    }
    else if( ugao == 90  ) 
    {
        napuni_trougao_p( vrhx, vrhy, vrhx - 10.0, vrhy - 10.0, vrhx + 10.0, vrhy - 10.0, -10, -15, 10, -15, 1, 0 );
    }
    else if( ugao == 180 ) 
    {
        napuni_trougao_p( vrhx, vrhy, vrhx - 10.0, vrhy - 10.0, vrhx - 10.0, vrhy + 10.0, -15, -10, -15, 10, 0, 1 );
    }
    else 
    {
        napuni_trougao_p( vrhx, vrhy, vrhx - 10.0, vrhy + 10.0, vrhx + 10.0, vrhy + 10.0, -10, 15, 10, 15, 1, 0 );
    }
}

void napuni_nepokretni( int vrhx, int vrhy, int ugao )
{
    if     ( ugao == 0 )   napuni_trougao( vrhx, vrhy, vrhx + 10.0, vrhy - 10.0, vrhx + 10.0, vrhy + 10.0 );
    else if( ugao == 90 )  napuni_trougao( vrhx, vrhy, vrhx - 10.0, vrhy - 10.0, vrhx + 10.0, vrhy - 10.0 );
    else if( ugao == 180 ) napuni_trougao( vrhx, vrhy, vrhx - 10.0, vrhy - 10.0, vrhx - 10.0, vrhy + 10.0 );
    else                   napuni_trougao( vrhx, vrhy, vrhx - 10.0, vrhy + 10.0, vrhx + 10.0, vrhy + 10.0 );
}


//------------------------------

void ImGui::ShowDemoWindow2(bool* p_open)
{
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

    static bool show_app_custom_rendering = true;
    static bool show_app_custom_rendering2 = true;
    static bool show_tool_metrics = false;
    static bool show_tool_debug_log = false;
    static bool show_tool_id_stack_tool = false;
    static bool show_tool_style_editor = false;
    static bool show_tool_about = false;

    if (show_tool_metrics)
        ImGui::ShowMetricsWindow(&show_tool_metrics);
    if (show_tool_debug_log)
        ImGui::ShowDebugLogWindow(&show_tool_debug_log);
    if (show_tool_id_stack_tool)
        ImGui::ShowIDStackToolWindow(&show_tool_id_stack_tool);
    if (show_tool_style_editor)
    {
        ImGui::Begin("Dear ImGui Style Editor", &show_tool_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (show_tool_about)
        ImGui::ShowAboutWindow(&show_tool_about);
    
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);


    ShowExampleAppCustomRendering(&show_app_custom_rendering);

    prikaziLeviProzor(&show_app_custom_rendering2);
}


static void prikaziLeviProzor( bool* p_open )
{
        //static float f = 0.0f;
        static bool alpha = true;
        static bool alpha_bar = true;
        static bool side_preview = true;
        static int picker_mode = 0;
        static int display_mode = 0;
        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;

        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoNav;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        p_open = NULL; 

        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
        
        if( ImGui::Begin("Korisnicki interfejs", p_open, window_flags) )
        {
            
            ImGui::Text( "GRID_STEP " );
            ImGui::SameLine();
            if( !brLinija )
            ImGui::InputInt( "##50", &GRID_STEP );


            const char* jedinice[] = { "mm", "cm", "dm", "m" };
            const char* ugaoOslonac[] = { "0", "90", "180", "270" };
            const char* ugaoSila[] = { "0", "45", "90", "135", "180", "225", "270", "315", "360" };
            const char* smerMom[] = { "Negativan", "Pozitivan" };
        
            const char* combo_preview_value1 = jedinice[mernaJedinica];
            const char* combo_preview_value2 = ugaoOslonac[indexUgaoOslonca];
            const char* combo_preview_value3 = ugaoSila[indexUgaoSile];
            const char* combo_preview_value4 = smerMom[smerMomenta];

            ImGuiColorEditFlags flags = misc_flags;
            if (!alpha)            flags |= ImGuiColorEditFlags_NoAlpha;       
            if (alpha_bar)         flags |= ImGuiColorEditFlags_AlphaBar;
            if (!side_preview)     flags |= ImGuiColorEditFlags_NoSidePreview;
            if (picker_mode == 1)  flags |= ImGuiColorEditFlags_PickerHueBar;
            if (picker_mode == 2)  flags |= ImGuiColorEditFlags_PickerHueWheel;
            if (display_mode == 1) flags |= ImGuiColorEditFlags_NoInputs;      
            if (display_mode == 2) flags |= ImGuiColorEditFlags_DisplayRGB;     
            if (display_mode == 3) flags |= ImGuiColorEditFlags_DisplayHSV;
            if (display_mode == 4) flags |= ImGuiColorEditFlags_DisplayHex;

            if (ImGui::BeginCombo(" ", combo_preview_value1, flags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(jedinice); n++)
                {
                    const bool is_selected = (mernaJedinica == n);
                    if (ImGui::Selectable(jedinice[n], is_selected))
                        mernaJedinica = n;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            static ImGuiTableFlags flags2 = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
            static ImVec2 cell_padding(0.0f, 0.0f);
            const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
            static bool show_widget_frame_bg = true;
            ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
            ImGui::Text( "\t   E\t\t"  ); ImGui::SameLine(); ImGui::Text( "\tA\t" ); ImGui::SameLine(); ImGui::Text( "\t\t I" );
            ImGui::SameLine();
                HelpMarker(
                    "- Unesite podatke za E, A, I, onim redosledom kojim ste dodavali stapove.\n"
                    "- Ako uklonite stap, izbrisite podatke iz tabele za taj stap\n");
            if (ImGui::BeginTable("table_padding_2", 3, flags2, outer_size))
            {

                static char text_bufs[3 * maxbrStapova][32];
                static bool init = true;



                if (!show_widget_frame_bg)
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
                for ( int cell = 0; cell < 3 * maxbrStapova; cell++ )
                {
                    ImGui::TableNextColumn();
                    if ( init  && cell < maxbrStapova )
                    {
                        Stap pom1;
                        pom1.brStapa = cell;
                        strcpy( text_bufs[cell], " " );
                        stapovi.push_back( pom1 );
                    }                
                        
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::PushID(cell);
                    ImGui::InputText("##cell", text_bufs[cell], IM_ARRAYSIZE(text_bufs[cell]));
                    ImGui::PopID();                      

                    if( !init && strcmp( text_bufs[cell], " " ) )
                    {
                        Stap pom;
                        int red, kolona;

                        red = cell / 3;
                        kolona = cell % 3;
                        
                        if( kolona == 0 ) stapovi[red].E = ( float ) atof( text_bufs[cell] );
                        if( kolona == 1 ) stapovi[red].A = ( float ) atof( text_bufs[cell] );
                        if( kolona == 2 ) stapovi[red].I = ( float ) atof( text_bufs[cell] );
                    }
                }

                if (!show_widget_frame_bg)
                    ImGui::PopStyleColor();
                init = false;
                ImGui::EndTable();
            }
            
            if( brLinija > 1 )
            {
                niz = ( Stap * ) realloc( niz, sizeof( Stap ) * brLinija );
            } 

        ImGui::RadioButton("Stap", &indikatorCrtanja, 0); ImGui::SameLine();
        ImGui::RadioButton("P.Oslonac", &indikatorCrtanja, 1); ImGui::SameLine();
        ImGui::RadioButton("N.Oslonac", &indikatorCrtanja, 2); 
        ImGui::RadioButton("Sila", &indikatorCrtanja, 3); ImGui::SameLine();
        ImGui::RadioButton("Momenat", &indikatorCrtanja, 4); 

        if (ImGui::BeginCombo("Ugao Oslonca", combo_preview_value2, flags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(ugaoOslonac); n++)
            {
                const bool is_selected = (indexUgaoOslonca == n);
                if (ImGui::Selectable(ugaoOslonac[n], is_selected))
                    indexUgaoOslonca = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Ugao Sile", combo_preview_value3, flags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(ugaoSila); n++)
            {
                const bool is_selected = (indexUgaoSile == n);
                if (ImGui::Selectable(ugaoSila[n], is_selected))
                    indexUgaoSile = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Smer Momenta", combo_preview_value4, flags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(smerMom); n++)
            {
                const bool is_selected = (smerMomenta == n);
                if (ImGui::Selectable(smerMom[n], is_selected))
                    smerMomenta = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text( "Intenzitet Sile" ); ImGui::SameLine(); HelpMarker("Pre crtanja sile, unesite njen intenzitet.\n");
        ImGui::InputInt( "##100", &intenzitetSile );

        ImGui::Text( "Intenzitet Momenta" ); ImGui::SameLine(); HelpMarker("Pre crtanja sile, unesite njen intenzitet.\n");
        ImGui::InputInt( "##101", &intenzitetMomenta );

        if (ImGui::Button("Izvezi fajl")) prikazPodataka();


        struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1; } };
        ImGuiKey start_key = (ImGuiKey)0;
        ImGuiKey key;
        for (key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) 
        { 
            if ( funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key) ) continue;
            if( key == 562 ) exit(1);
        }

        ImGui::Text( "Pritisnite taster q, da biste zatvorili prozor" );

        

        ImGui::End();
    } 
}  

static void ShowExampleAppCustomRendering(bool* p_open)
{
    if (!ImGui::Begin("Example: Custom rendering", p_open, window_flags))
    {
        ImGui::End();
        return;
    }
    IMGUI_DEMO_MARKER("Examples/Custom Rendering");
    
            static ImVec2 scrolling(0.0f, 0.0f);
            static bool opt_enable_grid = true;
            static bool opt_enable_context_menu = true;
            static bool adding_line = false;
            static ImVec4 col = ImVec4(1.0f, 0.5f, 0.4f, 1.0f);
            int x_trenutno, y_trenutno;
            float x_ostatak, y_ostatak;
        
            ImGui::Checkbox("Omoguci mrezu", &opt_enable_grid);
            ImGui::Text("Levi kliktaj misa: pritisni i povuci za crtanje, ali prvo selektuj sta zelis da crtas,\nDesni kliktaj misa: pritisni za padajuci meni.");


            ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      
            ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   
            if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
            if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
            ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

            ImGuiIO& io = ImGui::GetIO();
            draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
            draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

            ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft );
            const bool is_hovered = ImGui::IsItemHovered(); 
            const bool is_active = ImGui::IsItemActive();  
            const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); 
            ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

            if( indikatorCrtanja == 0 )
            {
                if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    sredinaX = sredinaY = 0;
                    x_trenutno = ( (int)mouse_pos_in_canvas.x / GRID_STEP ) * GRID_STEP;
                    y_trenutno = ( (int)mouse_pos_in_canvas.y / GRID_STEP ) * GRID_STEP;
                    x_ostatak = mouse_pos_in_canvas.x - x_trenutno;
                    y_ostatak = mouse_pos_in_canvas.y - y_trenutno;

                    if( x_ostatak >  GRID_STEP / 2 ) x_trenutno += GRID_STEP;
                    if( x_ostatak == GRID_STEP / 2 ) sredinaX = 1;    

                    if( y_ostatak >  GRID_STEP / 2 ) y_trenutno += GRID_STEP;
                    if( y_ostatak == GRID_STEP / 2 ) sredinaY = 1;


                    if( !sredinaX || !sredinaY )
                    {
                        mouse_pos_in_canvas.x = x_trenutno;
                        mouse_pos_in_canvas.y = y_trenutno;

                        if( brLinija == 0 )
                        {
                            points.push_back(mouse_pos_in_canvas);
                            points.push_back(mouse_pos_in_canvas);
                        }
                        if( brLinija >= 1 )
                        {
                            int x_tr = io.MousePos.x - origin.x;
                            int y_tr = io.MousePos.y - origin.y;
                            for( int i = 0; i < points.Size; i++ )
                            {
                                int pom = sqrt((x_tr-points[i].x)*(x_tr-points[i].x)+(y_tr-points[i].y)*(y_tr-points[i].y));
                                if( pom < GRID_STEP / 2 )
                                {
                                    const ImVec2 mouse_pos_in_canvas2(points[i].x, points[i].y);
                                    points.push_back(mouse_pos_in_canvas2);
                                    points.push_back(mouse_pos_in_canvas2);
                                    mis = 1;
                                    break;
                                };
                            }
                        }

                        adding_line = true;
                    }
                    else adding_line = false;
                }
                
                if (adding_line)
                {
                    if( mis == 0 && brLinija > 0 ) adding_line = false;
                    else if( brLinija > 0 )
                    {
                        int x_tr = io.MousePos.x - origin.x;
                        int y_tr = io.MousePos.y - origin.y;
                        const ImVec2 mouse_pos_in_canvas4 = points.back();

                        if( sqrt((x_tr-mouse_pos_in_canvas4.x)*(x_tr-mouse_pos_in_canvas4.x)+(y_tr-mouse_pos_in_canvas4.y)*(y_tr-mouse_pos_in_canvas4.y)) > GRID_STEP / 2 )
                        {
                            int x1_trenutno = ( (int)x_tr / GRID_STEP ) * GRID_STEP;
                            int y1_trenutno = ( (int)y_tr / GRID_STEP ) * GRID_STEP;
                            float x1_ostatak = x_tr - x1_trenutno;
                            float y1_ostatak = y_tr - y1_trenutno;
                            
                            if( x1_ostatak >  GRID_STEP / 2 ) x1_trenutno += GRID_STEP;
                            if( x1_ostatak == GRID_STEP / 2 ) sredinaX = 1;    

                            if( y1_ostatak >  GRID_STEP / 2 ) y1_trenutno += GRID_STEP;
                            if( y1_ostatak == GRID_STEP / 2 ) sredinaY = 1;

                            const ImVec2 mouse_pos_in_canvas5(x1_trenutno, y1_trenutno);


                            if( !misOtpusten ) points.back() = mouse_pos_in_canvas5;
                        };
                    }
                    else
                    {
                        int x_tr = io.MousePos.x - origin.x;
                        int y_tr = io.MousePos.y - origin.y;
                        const ImVec2 mouse_pos_in_canvas4 = points.back();

                        if( sqrt((x_tr-mouse_pos_in_canvas4.x)*(x_tr-mouse_pos_in_canvas4.x)+(y_tr-mouse_pos_in_canvas4.y)*(y_tr-mouse_pos_in_canvas4.y)) > GRID_STEP / 2 )
                        {
                            int x1_trenutno = ( (int)x_tr / GRID_STEP ) * GRID_STEP;
                            int y1_trenutno = ( (int)y_tr / GRID_STEP ) * GRID_STEP;
                            float x1_ostatak = x_tr - x1_trenutno;
                            float y1_ostatak = y_tr - y1_trenutno;
                            
                            if( x1_ostatak >  GRID_STEP / 2 ) x1_trenutno += GRID_STEP;
                            if( x1_ostatak == GRID_STEP / 2 ) sredinaX = 1;    

                            if( y1_ostatak >  GRID_STEP / 2 ) y1_trenutno += GRID_STEP;
                            if( y1_ostatak == GRID_STEP / 2 ) sredinaY = 1;

                            const ImVec2 mouse_pos_in_canvas5(x1_trenutno, y1_trenutno);
                            points.back() = mouse_pos_in_canvas5;
                        };
                    } 
                    if( brLinija == 0 && !ImGui::IsMouseDown(ImGuiMouseButton_Left) )
                    { 
                        adding_line = false;
                        brLinija++;
                    }
                    else if( !ImGui::IsMouseDown(ImGuiMouseButton_Left) )
                    {
                        adding_line = false;
                        brLinija++;
                        mis = 0;
                        misOtpusten = 0;
                    }      
                }
            }

            //-------------------------------------

            if( indikatorCrtanja > 0 )
            {
                int x_tr = io.MousePos.x - origin.x;
                int y_tr = io.MousePos.y - origin.y;
                int x_centar = 0, y_centar = 0;
                bool nasao = 0;

                for( int i = 0; i < points.Size && !nasao; i++ )
                {
                    int pom = sqrt((x_tr-points[i].x)*(x_tr-points[i].x)+(y_tr-points[i].y)*(y_tr-points[i].y));
                    if( pom <= 25 )
                    {
                        x_centar = points[i].x;
                        y_centar = points[i].y;
                        nasao = 1;
                    };
                }

                if( nasao && brLinija && is_hovered && indikatorCrtanja == 1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
                {
                    bool indikator = 1;
                    int i;
                    for( i = 0 ; i < pokretni.Size && indikator; i++ )
                    {
                        if( x_centar + 345 == pokretni[i].t0.x && y_centar + 80 == pokretni[i].t0.y ) indikator = 0;
                    }
                    if( indikator ) napuni_pokretni( x_centar, y_centar, ugaoOslonca[indexUgaoOslonca] );
                };

                if( nasao && brLinija && is_hovered && indikatorCrtanja == 2 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
                {
                    bool indikator = 1;
                    for( int i = 0 ; i < nepokretni.Size && indikator; i++ )
                    {
                        if( x_centar + 345 == nepokretni[i].t0.x && y_centar + 80 == nepokretni[i].t0.y ) indikator = 0;
                    }
                    if( indikator ) napuni_nepokretni( x_centar, y_centar, ugaoOslonca[indexUgaoOslonca] );
                };

                 if( nasao && brLinija && is_hovered && indikatorCrtanja == 3 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
                 {
                    bool indikator = 1;
                    int i;
                    for( i = 0 ; i < sile.Size && indikator; i++ )
                    {
                        if( x_centar + 345 == sile[i].poc.x && y_centar + 80 == sile[i].poc.y ) indikator = 0;
                    }
                    if( indikator ) strelica( x_centar, y_centar, ugaoSile[indexUgaoSile] );
                 };

                 if( nasao && brLinija && is_hovered && indikatorCrtanja == 4 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
                 {
                    bool indikator = 1;
                    for( int i = 0 ; i < momenti.Size && indikator; i++ )
                    {
                        if( x_centar + 350 == momenti[i].centar.x && y_centar + 80 == momenti[i].centar.y ) indikator = 0;
                    }
                    if( indikator ) nacrtaj_moment( x_centar, y_centar );
                 };
            }

            //-------------------------------------

            const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
            if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
            {
                scrolling.x += io.MouseDelta.x;
                scrolling.y += io.MouseDelta.y;
            }

            ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            if (opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
                ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
            if (ImGui::BeginPopup("context"))
            {
                if (adding_line)
                    points.resize(points.size() - 2);
                adding_line = false;
                if (ImGui::MenuItem("Ukloni sve", NULL, false, points.Size > 0)) { points.clear(); brLinija = 0; pokretni.clear(); nepokretni.clear(); sile.clear(); momenti.clear();}
                if (ImGui::MenuItem("Ukloni jedan stap", NULL, false, points.Size > 0)) { points.resize(points.size() - 2); brLinija--; }
                if (ImGui::MenuItem("Ukloni sve stapove", NULL, false, points.Size > 0)) { points.clear(); brLinija = 0; }
                if (ImGui::MenuItem("Ukloni jedan Poslonac", NULL, false, pokretni.Size > 0)) { pokretni.resize(pokretni.size() - 1); }
                if (ImGui::MenuItem("Ukloni sve Poslonce", NULL, false, pokretni.Size > 0)) { pokretni.clear(); }
                if (ImGui::MenuItem("Ukloni jedan Noslonac", NULL, false, nepokretni.Size > 0)) { nepokretni.resize(nepokretni.size() - 1); }
                if (ImGui::MenuItem("Ukloni sve Noslonce", NULL, false, nepokretni.Size > 0)) { nepokretni.clear(); }
                if (ImGui::MenuItem("Ukloni jednu silu", NULL, false, sile.Size > 0)) { sile.resize(sile.size() - 1); }
                if (ImGui::MenuItem("Ukloni sve sile", NULL, false, sile.Size > 0)) { sile.clear(); }
                if (ImGui::MenuItem("Ukloni jednan momenat", NULL, false, momenti.Size > 0)) { momenti.resize(momenti.size() - 1); }
                if (ImGui::MenuItem("Ukloni sve momente", NULL, false, momenti.Size > 0)) { momenti.clear(); }
                ImGui::EndPopup();
            }

            draw_list->PushClipRect(canvas_p0, canvas_p1, true);
            if (opt_enable_grid)
            {
                for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
                for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
            }

            for (int n = 0; n < points.Size; n += 2)
                draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0 );
            const ImU32 col32 = ImColor(col);
            for (int n = 0; n < points.Size; n += 1)
                draw_list->AddCircle(ImVec2(origin.x + points[n].x, origin.y + points[n].y), 5, col32, 20, 1);

            for ( int i = 0; i < pokretni.Size; i++ )
            {
                draw_list -> AddTriangleFilled( pokretni[i].t0, pokretni[i].t1, pokretni[i].t2, col32 );
                draw_list -> AddLine( pokretni[i].poc, pokretni[i].kraj, col32, 2.0F );
            }

            for ( int i = 0; i < nepokretni.Size; i++ )
            {
                draw_list -> AddTriangleFilled( nepokretni[i].t0, nepokretni[i].t1, nepokretni[i].t2, col32 );
            }

            for ( int i = 0; i < sile.Size; i++ )
            {
                draw_list -> AddTriangleFilled( sile[i].t0, sile[i].t1, sile[i].t2, col32 );
                draw_list -> AddLine( sile[i].poc, sile[i].kraj, col32, 4.0f );
            }

            for ( int i = 0; i < momenti.Size; i++ )
            {
                draw_list -> PathArcTo( momenti[i].centar, 15.0F, -M_PI/2, M_PI / 2 , 10 );

                draw_list->PathStroke(ImGui::GetColorU32(ImGuiCol_Text));

                draw_list -> AddTriangleFilled( momenti[i].t1, momenti[i].t2, momenti[i].t3, col32 );
            }
            
    draw_list->PopClipRect();
    ImGui::End();
}

namespace ImGui { IMGUI_API void ShowFontAtlas(ImFontAtlas* atlas); }

void ImGui::ShowStyleEditor(ImGuiStyle* ref)
{
    IMGUI_DEMO_MARKER("Tools/Style Editor");
    ImGuiStyle& style = ImGui::GetStyle();
    static ImGuiStyle ref_saved_style;

    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

    if (ImGui::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    ImGui::ShowFontSelector("Fonts##Selector");

    if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding;
    { bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder",  &border)) { style.FrameBorderSize  = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder",  &border)) { style.PopupBorderSize  = border ? 1.0f : 0.0f; } }

    if (ImGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    ImGui::SameLine();
    if (ImGui::Button("Revert Ref"))
        style = *ref;
    ImGui::SameLine();
    HelpMarker(
        "Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
        "Use \"Export\" below to save them somewhere.");

    ImGui::Separator();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Sizes"))
        {
            ImGui::SeparatorText("Main");
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");

            ImGui::SeparatorText("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBarBorderSize", &style.TabBarBorderSize, 0.0f, 2.0f, "%.0f");

            ImGui::SeparatorText("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");

            ImGui::SeparatorText("Tables");
            ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderAngle("TableAngledHeadersAngle", &style.TableAngledHeadersAngle, -50.0f, +50.0f);

            ImGui::SeparatorText("Widgets");
            ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = window_menu_button_position - 1;
            ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
            ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
            ImGui::SliderFloat("SeparatorTextBorderSize", &style.SeparatorTextBorderSize, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat2("SeparatorTextAlign", (float*)&style.SeparatorTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat2("SeparatorTextPadding", (float*)&style.SeparatorTextPadding, 0.0f, 40.0f, "%.0f");
            ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");

            ImGui::SeparatorText("Tooltips");
            for (int n = 0; n < 2; n++)
                if (ImGui::TreeNodeEx(n == 0 ? "HoverFlagsForTooltipMouse" : "HoverFlagsForTooltipNav"))
                {
                    ImGuiHoveredFlags* p = (n == 0) ? &style.HoverFlagsForTooltipMouse : &style.HoverFlagsForTooltipNav;
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNone", p, ImGuiHoveredFlags_DelayNone);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayShort", p, ImGuiHoveredFlags_DelayShort);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNormal", p, ImGuiHoveredFlags_DelayNormal);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_Stationary", p, ImGuiHoveredFlags_Stationary);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_NoSharedDelay", p, ImGuiHoveredFlags_NoSharedDelay);
                    ImGui::TreePop();
                }

            ImGui::SeparatorText("Misc");
            ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f"); ImGui::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (ImGui::Button("Export"))
            {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const ImVec4& col = style.Colors[i];
                    const char* name = ImGui::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
                            name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                ImGui::LogFinish();
            }
            ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = 0;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
            if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            HelpMarker(
                "In the color list:\n"
                "Left-click on color square to open color picker,\n"
                "Right-click to open edit options menu.");

            ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
            ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
            ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                if (ImGui::Button("?"))
                    ImGui::DebugFlashStyleColor((ImGuiCol)i);
                ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
                ImGui::SameLine();
                ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                {
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
                }
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Fonts"))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
            ImGui::ShowFontAtlas(atlas);

            const float MIN_SCALE = 0.3f;
            const float MAX_SCALE = 2.0f;
            HelpMarker(
                "Those are old settings provided for convenience.\n"
                "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
                "rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
                "Using those settings here will give you poor quality results.");
            static float window_scale = 1.0f;
            ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
            if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp)) 
                ImGui::SetWindowFontScale(window_scale);
            ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rendering"))
        {
            ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            ImGui::SameLine();
            HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

            ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
            ImGui::SameLine();
            HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

            ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
            ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

            ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            const bool show_samples = ImGui::IsItemActive();
            if (show_samples)
                ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
            if (show_samples && ImGui::BeginTooltip())
            {
                ImGui::TextUnformatted("(R = radius, N = number of segments)");
                ImGui::Spacing();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                const float min_widget_width = ImGui::CalcTextSize("N: MMM\nR: MMM").x;
                for (int n = 0; n < 8; n++)
                {
                    const float RAD_MIN = 5.0f;
                    const float RAD_MAX = 70.0f;
                    const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

                    ImGui::BeginGroup();

                    ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

                    const float canvas_width = IM_MAX(min_widget_width, rad * 2.0f);
                    const float offset_x     = floorf(canvas_width * 0.5f);
                    const float offset_y     = floorf(RAD_MAX);

                    const ImVec2 p1 = ImGui::GetCursorScreenPos();
                    draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
                    ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

                    ImGui::EndGroup();
                    ImGui::SameLine();
                }
                ImGui::EndTooltip();
            }
            ImGui::SameLine();
            HelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");

            ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f");
            ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); HelpMarker("Additional alpha multiplier for disabled items (multiply over current value of Alpha).");
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopItemWidth();
}

void ImGui::ShowAboutWindow(bool* p_open)
{
    if (!ImGui::Begin("About Dear ImGui", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }
    IMGUI_DEMO_MARKER("Tools/About Dear ImGui");
    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    ImGui::Separator();
    ImGui::Text("By Omar Cornut and all Dear ImGui contributors.");
    ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
    ImGui::Text("If your company uses this, please consider sponsoring the project!");

    static bool show_config_info = false;
    ImGui::Checkbox("Config/Build Information", &show_config_info);
    if (show_config_info)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        bool copy_to_clipboard = ImGui::Button("Copy to clipboard");
        ImVec2 child_size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 18);
        ImGui::BeginChild(ImGui::GetID("cfg_infos"), child_size, ImGuiChildFlags_FrameStyle);
        if (copy_to_clipboard)
        {
            ImGui::LogToClipboard();
            ImGui::LogText("```\n"); 
        }

        ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
        ImGui::Separator();
        ImGui::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
        ImGui::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_OBSOLETE_KEYIO
        ImGui::Text("define: IMGUI_DISABLE_OBSOLETE_KEYIO");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_FILE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_ALLOCATORS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
        ImGui::Text("define: IMGUI_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
        ImGui::Text("define: _WIN32");
#endif
#ifdef _WIN64
        ImGui::Text("define: _WIN64");
#endif
#ifdef __linux__
        ImGui::Text("define: __linux__");
#endif
#ifdef __APPLE__
        ImGui::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
        ImGui::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef _MSVC_LANG
        ImGui::Text("define: _MSVC_LANG=%d", (int)_MSVC_LANG);
#endif
#ifdef __MINGW32__
        ImGui::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
        ImGui::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
        ImGui::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
        ImGui::Text("define: __clang_version__=%s", __clang_version__);
#endif
#ifdef __EMSCRIPTEN__
        ImGui::Text("define: __EMSCRIPTEN__");
#endif
        ImGui::Separator();
        ImGui::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
        ImGui::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
        ImGui::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)        ImGui::Text(" NavEnableKeyboard");
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)         ImGui::Text(" NavEnableGamepad");
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableSetMousePos)     ImGui::Text(" NavEnableSetMousePos");
        if (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard)     ImGui::Text(" NavNoCaptureKeyboard");
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)                  ImGui::Text(" NoMouse");
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)      ImGui::Text(" NoMouseCursorChange");
        if (io.MouseDrawCursor)                                         ImGui::Text("io.MouseDrawCursor");
        if (io.ConfigMacOSXBehaviors)                                   ImGui::Text("io.ConfigMacOSXBehaviors");
        if (io.ConfigInputTextCursorBlink)                              ImGui::Text("io.ConfigInputTextCursorBlink");
        if (io.ConfigWindowsResizeFromEdges)                            ImGui::Text("io.ConfigWindowsResizeFromEdges");
        if (io.ConfigWindowsMoveFromTitleBarOnly)                       ImGui::Text("io.ConfigWindowsMoveFromTitleBarOnly");
        if (io.ConfigMemoryCompactTimer >= 0.0f)                        ImGui::Text("io.ConfigMemoryCompactTimer = %.1f", io.ConfigMemoryCompactTimer);
        ImGui::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
        if (io.BackendFlags & ImGuiBackendFlags_HasGamepad)             ImGui::Text(" HasGamepad");
        if (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors)        ImGui::Text(" HasMouseCursors");
        if (io.BackendFlags & ImGuiBackendFlags_HasSetMousePos)         ImGui::Text(" HasSetMousePos");
        if (io.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset)   ImGui::Text(" RendererHasVtxOffset");
        ImGui::Separator();
        ImGui::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
        ImGui::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        ImGui::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        ImGui::Separator();
        ImGui::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
        ImGui::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
        ImGui::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
        ImGui::Text("style.FrameRounding: %.2f", style.FrameRounding);
        ImGui::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
        ImGui::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
        ImGui::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

        if (copy_to_clipboard)
        {
            ImGui::LogText("\n```\n");
            ImGui::LogFinish();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

bool ImGui::ShowStyleSelector(const char* label)
{
    static int style_idx = -1;
    if (ImGui::Combo(label, &style_idx, "Dark\0Light\0Classic\0"))
    {
        switch (style_idx)
        {
        case 0: ImGui::StyleColorsDark(); break;
        case 1: ImGui::StyleColorsLight(); break;
        case 2: ImGui::StyleColorsClassic(); break;
        }
        return true;
    }
    return false;
}

void ImGui::ShowFontSelector(const char* label)
{
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font_current = ImGui::GetFont();
    if (ImGui::BeginCombo(label, font_current->GetDebugName()))
    {
        for (ImFont* font : io.Fonts->Fonts)
        {
            ImGui::PushID((void*)font);
            if (ImGui::Selectable(font->GetDebugName(), font == font_current))
                io.FontDefault = font;
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    HelpMarker(
        "- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
        "- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
        "- Read FAQ and docs/FONTS.md for more details.\n"
        "- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");
}

#else

void ImGui::ShowAboutWindow(bool*) {}
void ImGui::ShowDemoWindow(bool*) {}
void ImGui::ShowUserGuide() {}
void ImGui::ShowStyleEditor(ImGuiStyle*) {}

#endif
#endif // #ifndef IMGUI_DISABLE