/* 
 * Starten wir mit dem einfachsten.
 * 
 * Wir brauchen die Ausgabe auf die Konsole und SDL2.
 * Also includen wir diese erstmal hier.
 * */
#include <iostream>
/*
 * Hier unter Linux liegt SDL.h in /usr/include/SDL2/
 * Über CMake wird dieser Pfad automatisch mit als Include-Pfad an den Kompiler
 * übergeben.
 * BMP sind für den Anfang OK. Aber PNG Bilder bieten doch etwas mehr komfort.
 * Um diese Nutzen zu können, brauchen wir aber die SDL_image Bibliothek.
 * */ 
#include <SDL.h>
#include <SDL_image.h>

/*
 * Falls Fehler auftreten, werfen wir einfach mit exceptions um uns. Irgendwer
 * wird sie schon fangen. Hier in den Includes sind ein paar Ausnahmen
 * definiert, die wir später einfach nutzen werden.
 * */
#include <exception>
#include <stdexcept>

// Die Array-Klasse.
// Es ist ein Array statischer Größe.
// Vector ist auch eine Array-Klasse, aber mit dynamischer Größe.
// Es kann während der Laufzeit seine Größe anpassen, neue Elemente bekommen
// oder welche entfernen.
#include <array>
#include <vector>

// Natürlich könnte man auch einfach die alten * Zeiger verwenden.
// SmartPointer sind doch aber etwas angenehmer. Sie passen auf den Inhalt auf
// und sorgen sich etwas um die Speicherbereinigung.
#include <memory>

// Für ein paar Mathefunktionen
#include <cmath>

/*
 * Das hier ist eine kleine Hilfsfunktion.
 * An sich ist sie wohl auch als ASSERT bekannt.
 * Man nimmt an, dass etwas stimmt. Und wenn nicht, dann ist es schlimm. In
 * diesem Fall nutzen wir die Funktion für SDL Funktionen. Sollte jene kritisch
 * sein und fehlschlagen, dann werfen wir eine Exception mit dem passenden
 * Fehler.
 * */
void SDL_ANNAHME( bool stimmt){
	if(!stimmt){
		throw std::runtime_error(std::string() + SDL_GetError());
	}
}

/*
 * Eine plumpe Kopie der Zeilen oben drüber.
 * (Vielleicht später geschickt ummodeln.)
 * */
void IMG_ANNAHME( bool stimmt){
	if(!stimmt){
		throw std::runtime_error(std::string() + IMG_GetError());
	}
}


/*
 * Was wollen wir...
 * → Einheiten bewegen
 * Dazu brauchen wir:
 * → eine Bewegung pro Frame
 * → eine "Einheit"
 * → ein Weg
 *
 * Egal wie viele Frames man pro Sekunde hat, soll sich eine Einheit gleich
 * bewegen. Die Bewegung muss also Abhängig von den Frames je Sekunde sein.
 * Je mehr FPS, desto kleinere Schritte.
 *
 * Jetzt basteln wir uns eine Einheit.
 * Das wird eine eigene Klasse.
 * → Update soll jeden Frame aufgerufen werden und die Position neu berechnen
 * → Draw soll die Einheit zeichnen
 * Beide müssen public sein, damit sie von außen aufgerufen werden können.
 *
 * Erstmal haben wir einen ziemlich einfachen Weg. Er geht nur von 0,0 nach
 * 1024,768.
 * Es wäre aber praktisch, wenn wir einen beliebigen Weg laufen könnten. Immer
 * in Teilabschnitte - versteht sich.
 * Waypoints machen sich da nicht schlecht.
 * Wir brauchen also eine Liste von Wegpunkten.
 * Und dann laufen wir diese ab.
 * Viele Einheiten sollen später auf dem gleichen Weg laufen. Also ist es
 * besser, wie auch bei der Texture, dass die Einheiten sich die Wegpunkte
 * teilen. 
 * Die Einheit muss also nur wissen:
 * → Wo ist die Liste mit Wegpunkten
 * → Wo ist mein nächster Punkt
 * 
 * Gut. Das wäre das.
 * */

/*
 * Faulheit!
 * Wir erstellen ein paar Synonyme.
 * Es ist kürzer einfach 'Point' als Type zu verwenden, als dieses std::array...
 * Auch der Zeiger würde ausgeschrieben um einiges länger sein, und ggf. etwas
 * unverständlich. So kann man dem Typ auch eine kleine Bedeutung anhängen.
 * */
typedef std::array<int, 2> Point;
typedef std::vector<Point> WaypointList;
typedef std::shared_ptr<WaypointList> WaypointListZeiger;

// Der shared_ptr ist ein Konstrukt, der im Prinzip ein * Zeiger ist.
// Er sorgt aber dafür, dass, sobald keiner mehr den Zeiger verwendet, dieser
// gelöscht wird. Man muss also nicht acht geben, wann man wohlmöglich ein
// 'delete' setzen müsste. 

class Einheit {
	public:
        // Hier sind wir richtig.
		// Es gibt ein paar kleine Methoden für eine Klasse, die an speziellen
		// Orten aufgerufen werden. 
		// Man kenn sie als Konstruktoren
		// Es gibt ein paar davon.
		// Gerade interessieren wir uns für den Copy-Constructor
		// Mit diesem sehen wir, dass wir unten in die Liste nicht das Objekt
		// selber, sondern eine Kopie davon reinstecken.
		//
		// C++ baut automatisch diese Konstruktoren, wenn er es kann.
		// Da wir nichts "seltsames" nutzen, hat uns C++ die Konstruktoren
		// passend eingesetzt.
		// Jetzt bauen wir uns einen eigenen.
		// 
		// Kopierkonstruktor
		// Wir erstellen also eine Kopie. Von einem Objekt, dass vom gleichen
		// Typ ist.
		// Das ist die Zuweisung von Werten zu unserem Objekt
		// Wir übernehmen die Werte, die die einheit (Parameter) hat.
		// An sich könnten wir das auch innerhalb der Methode, also in den {}
		// machen, aber hier oben nach dem : werden die Werte sowieso festgelegt
		// Schreiben wir also erst in den {} Werte in die Variablen, so werden
		// diese 2 mal geschrieben. Einmal oben nach dem : und einmal im {}
		//
		Einheit( const Einheit &einheit)
			: m_texture(einheit.m_texture)
			, m_rect(einheit.m_rect)
			, m_zielPosition(einheit.m_zielPosition)
			, m_restBewegung( einheit.m_restBewegung)
			, m_geschwindigkeit(einheit.m_geschwindigkeit)
			, m_naechsterWegpunktID(einheit.m_naechsterWegpunktID)
			, m_naechsterWegpunkt(einheit.m_naechsterWegpunkt)
			, m_alleWegpunkte(einheit.m_alleWegpunkte)
			, m_leben(einheit.m_leben)
		{
			// Lassen wir das Programm jetzt laufen, so sehen wir: 
			// der Konstruktor unten wird nur ein mal aufgerufen
			// der Kopier-Konstruktor hier jedoch öfter. Und zwar genau dann,
			// wenn wir eine neue Einheit in die Liste der aktiven Einheiten
			// stecken.
			std::clog << "[Einheit] Kopier-Konstruktor" << std::endl;
		}

		// Das hier ist der ganz normale Konstruktor.
		// Er wird aufgerufen bei zB: 
		//		Einheit e;
		Einheit(){
			std::clog << "[Einheit] Konstruktor" << std::endl;
		};

		// Wir geben nichts zurück.
		// Aber wir brauchen die Information, wie viel Zeit für den Frame
		// verstrichen ist. Je größer die Zeit, desto weiter müssen wir uns
		// bewegen. Heißt aber auch, dass nur wenig FPS da sein werden.
		// Sind viele FPS da, wird die Zeit pro Frame kleiner, und wir bewegen
		// uns in kleineren Schritten.
		void update( int frameZeit){

			// In m_rect steckt x und y, welche wir nutzen können um zu sagen,
			// an welcher Position wir sind und die Texture zeichnen wollen
			//
			// Wir sind an x,y und wollen zum Ziel.
			// Nehmen wir Vektoren.
			// Ziel - Anfang == Weg
			//
			// Wir brauche double für die weiteren Berechnungen. 

			double wegX = m_naechsterWegpunkt[0] - m_rect.x;
			double wegY = m_naechsterWegpunkt[1] - m_rect.y;

            // Wir sollen insgesamt eine Stecke ablaufen von ?
			// Das dürfte der Betrag des Insgesamtweg Vektors sein
			// also von 0,0 nach 1024,768
			// Vektorrechnung!
			// Die Wurzel aus a² + b²
			//auto gesamtWeg = std::sqrt( 1024*1024 + 768*768);

			// Den Weg in 3 Sekunden, also 3000ms
			// das sind dann also 
			// 4.2666667
			//auto pixelProSekunde = gesamtWeg / (m_speed * 1000);


            // Unser restlicher Weg ist mit wegX,Y gegeben.
			// Wie lang ist dieser?
			auto wegLaenge = std::sqrt( wegX*wegX + wegY*wegY);
			

			// Ist der Weg größer, als das, was wir innerhalb der Zeit schaffen
			// würden, dann dürfen wir nicht soweit gehen.
			// Pro MS dürfen wir maximal 4 Pixel gehen. Das legen wir jetzt
			// einfach so fest. 
			// Wir werden pro Frame nicht immer genau eine Millisekunde Zeit
			// haben. Also können wir auch nicht immer nur maximal 4 Schritte
			// gehen. Haben wir mehr Zeit gebraucht, gehen wir weiter. Wir
			// müssen also die 4 mit der Zeit für einen Frame multiplizieren.
			// Speed ist zur Zeit die angabe, wie lange die Einheit für einen
			// "Streckenabschnitt" benötigen darf. 1 heißt 1Sekunde, 5 sind 5
			// Sekunden.
			// Besser ist eine Geschwindigkeit in Pixel pro Millisekunde.
			//  s = v*t --- weg ist geschwindigkeit * zeit
			// Je kleiner die Geschwindigkeit desto kürzer der Weg in gleicher
			// Zeit.
			//
			auto derWeg = m_geschwindigkeit * frameZeit;
			if( wegLaenge > derWeg){
				// Ist der Weg zu lang, dann kürzen wir ihn soweit, dass es
				// genau 4 Pixel werden.
				auto scale = derWeg/wegLaenge;
				wegX *= scale;
				wegY *= scale;
			}else{
				// Wir schaffen des letzten Rest in einem Zug!
				// Wir können also uns danach auf den nächsten Wegpunkt stürzen
				// Vorher müssen wir aber feststellen, ob es noch einen nächsten
				// Wegpunkt gibt oder ob wir schon am Ziel sind.
				if( m_naechsterWegpunktID < m_alleWegpunkte->size()){
					m_naechsterWegpunkt = m_alleWegpunkte->at( m_naechsterWegpunktID);
					++m_naechsterWegpunktID;
				}
			}

			// Was hinter dem Komma kommt, das schmeißen wir mit in die
			// restliche Bewegung rein.
			int iwegX = (int)std::floor(wegX);
			int iwegY = (int)std::floor(wegY);
			m_restBewegung[0] += (wegX - iwegX);
			m_restBewegung[1] += (wegY - iwegY);


			// Ist die restliche Bewegung größer gleich einem Pixel auf einer
			// Achse, dann nehmen wir den Pixel und packen ihn zu der Bewegung
			// mit hinzu.
			int restX = (int)std::floor(m_restBewegung[0]);
			int restY = (int)std::floor(m_restBewegung[1]);
			iwegX += restX;
			iwegY += restY;
			m_restBewegung[0] -= restX;
			m_restBewegung[1] -= restY;



			// pro Millisekunde also 4.266 Pixel
			// Schwierig. Es gibt selten Pixel, die kleiner 1 sind ;-)
			//
			// Die Pixel, die wir gehen dürfen, bewegen wir uns jetzt weiter.
			//

			m_rect.x += iwegX;
			m_rect.y += iwegY;
		
		}

		// Wir geben auch hier nichts zurück.
		// Aber wir müssen wissen, wo wir die Einheit hinmalen sollen, auf
		// welchen renderer. Deswegen übergeben wir diesen.
		// In der Main-Funktion liegt der renderer schon in einem Zeiger, also
		// nutzen wir das und verlangen hier auch einfach einen Zeiger.
		//
		// Wir kopieren die Texture an die passende Stelle auf die
		// Render-Fläche. Das haben wir ja schon weiter unten im Code gemacht.
		void draw(SDL_Renderer *renderer){
			SDL_RenderCopy(renderer, m_texture, nullptr, &m_rect);
		}

		// Irgendwoher müssen wir ja unsere Texture bekommen. In dem Fall
		// übergeben und setzen wir sie hier.
		// Diese Methode muss dann aber auch vor dem ersten draw aufgerufen
		// werden!
		// In der  Texture steckt die Größe nicht drin, also müssen wir die auch
		// noch erfahren.
		void init( SDL_Texture *texture, SDL_Rect rect){
			m_texture = texture;
			m_rect.x = rect.x;
			m_rect.y = rect.y;
			m_rect.w = rect.w;
			m_rect.h = rect.h;
		}


		// wir setzen die Liste der Wegpunkte
		void setzeWegpunkte( WaypointListZeiger wegpunkte){
			m_alleWegpunkte = wegpunkte;

			// Wir haben die Liste.
			// Also setzten wir doch gleich das erste Ziel
			m_naechsterWegpunkt = m_alleWegpunkte->at(0);
			m_naechsterWegpunktID = 1;
		}


		// An sich könnten wir so neue Wegpunkte in die Liste einfügen. Aber
		// hier brauchen wir das nicht.
		//void hinzufuegenWegpunkte( WaypointListZeiger wegpunkte){
			//m_alleWegpunkte->insert(m_alleWegpunkte->end(), wegpunkte->begin(), wegpunkte->end());
		//}
		
        Point getPosition(){
			return {{m_rect.x, m_rect.y}};
		}


		// Wir wurden getoffen!
		// Unser Leben sinkt...
		// Sind wir tot, leben <= 0, dann ist es vorbei
		bool gotHit(int damage){
			m_leben -= damage;
			return m_leben <= 0;
		}
	private:
		// Private heißt, dass nur *ich*, also die Klasse selber zugriff auf die
		// Variable oder Methode hat. Keiner kann diese von außen verändern oder
		// lesen. Nicht einmal abgeleitete Klassen.
		// 
		// Um während des draw etwas zeichnen zu können, müssen wir auch etwas
		// zeichenbares haben. Also am besten eine Texture.
		// Wir könnten nun eine eigene Texture für jede Einheit nehmen. Aber da
		// die Einheiten meist gleich sind, dürfen sie sich auch eine Texture
		// teilen. Das spart Speicherplatz.
		SDL_Texture *m_texture = nullptr;
		SDL_Rect m_rect{0,0,0,0};

		// Wir kennen ja noch die Größe des Fensters. Laufen wir also einfach
		// mal schräg rüber (sofern wir bei 0,0 starten sollten).
		// TODO natürlich müssen wir das nacher noch ordentlich machen.
		// FIXME Ich weiß, dass die Texture 32x32 groß ist. Die Größe ziehe ich
		// von der ZielPosition ab, damit ich unten rechts die Einheit auch noch
		// sehe.
		// 
		std::array<int,2> m_zielPosition{{1024-32,768-32}};

		// Wir bewegen uns ja immer in Pixeln. Die Berechnungen sind aber teis
		// Komma-Werte. Also müssen wir irgendwo das absichern, was wir
		// "abschneiden". Ist das abgeschnittene groß genug, hängen wir es an
		// die Bewegung dran.
		std::array<double,2> m_restBewegung{{0,0}};

		// Probieren wir mal in 3 Sekunden es über den Bildschirm zu schaffen.
		// Irgendwie war 4 viel zu schnell.
		// Mit 1 geht es besser. Stimmt aber jetzt nicht mehr als "Sekunden"
		//
		// So passt das. Die Gesamtstrecke ist ja gerade noch 1280 px. Die will
		// ich in 2 Sekunden bestreiten. Also 640 px pro Sekunde.
		// Die 2 kann ich jetzt einfach durch andere Sekunden erstetzen. Mehr
		// ist langsamer, weniger ist schneller.
		double m_geschwindigkeit = (1280.0 /2.0)/1000.0; // 0.320; // in px / ms


		uint32_t m_naechsterWegpunktID = 0;
		Point m_naechsterWegpunkt{{0,0}};
		WaypointListZeiger m_alleWegpunkte = nullptr;


		int m_leben = 5;
};


/*
 * Als nächstes der Tower.
 * Er ist fest.
 * Er schießt auf Gegner in einem bestimmten Radius.
 *
 * */
class Turm {
	public:
        void init( SDL_Texture *texture, SDL_Rect rect){
			m_texture = texture;
			m_rect = rect;
		}

		void update( int frameZeit){
			// TODO
			// Es wäre natürlich ganz praktisch noch ein paar Schüsse zu haben
			// Soll er doch bei jedem Update eine dazu bekommen ...
			// TODO Natürlich sollte hier eine Zeit eingesetzt werden.
			// Vielleicht nach jeder Sekunde oder so. Aber zum Testen sollte es
			// erstmal so gehen
			++m_shootsLeft;
		
		}

		void draw( SDL_Renderer *renderer){
			SDL_RenderCopy(renderer, m_texture, nullptr, &m_rect);
		}

        /*
		 * 
		 * 1) Haben wir noch Schuss übrig?
		 * 2) Schauen ob sich Einheit in Reichweite befindet
		 * 3) Schießen.
		 *
		 * */
		bool shoot( Einheit &einheit){
			if( m_shootsLeft <= 0) return false;
			auto ePos = einheit.getPosition();

			// Der Vektor von hier zum Gegner
			Point zielVector{{ ePos[0] - m_rect.x, ePos[1] - m_rect.y}};

			// Die länge des Weges quadriert.
			auto weg = zielVector[0]*zielVector[0] + zielVector[1]*zielVector[1];

			// Wir können das Quadrat nutzen, weil in unserem Fall
			// sqrt(a) <= sqrt(b)  auch gleich a <= b
			// wir sparen also die Berechnung der Wurzel
			// Tja, ist also der Weg bis zum Gegner größer als unsere
			// Reichweite, dann hören wir auf.
			if( weg > m_reichweite2) return false;

			// An dieser Stelle wissen wir:
			// - wir haben noch Schüsse übrig
			// - der Gegner ist in Reichweite
			// → also schießen wir
			// Wir haben dann einen Schuss weniger.
			// Der eigentliche Schuss wird an anderer Stelle behandelt.
			--m_shootsLeft;
			return true;
		}

		Point getPosition(){
			return {{m_rect.x, m_rect.y}};
		}
	private:
		SDL_Texture *m_texture = nullptr;
		SDL_Rect m_rect{0,0,0,0};
		//Point m_rotation; // Wo schaut er hin. Brauchen wir aber erstmal nicht.
		
		int m_shootsLeft = 0;
		//int m_maxShoots = 1; // FIXME: wird gerade nicht benutzt
		// Ein Feld ist jetzt mal 32px breit. Die Reichweite ist 5 Felder.
		int m_reichweite = 32*5;
		int m_reichweite2 = m_reichweite*m_reichweite; // das quadrat davon
		
};


/* Die Standard-Funktion eines jeden C++ Programms: main
 * Darf natürlich auch hier nicht fehlen.
 * Hier geht es los. Hier hört es auf.
 * Wird die main betreten, dann geht das Programm los.
 * Hört die main auf, hört das Programm auf.
 *
 * argc - Anzahl der Argumente, die in argv stecken
 * argv - Argumente. Diese können über die Kommando-Zeile hinzugefügt werden.
 * */
int main(int argc, char **argv){

	/*
	 * Legen wir ein paar Variablen an.
	 * Ein Zeiger auf einen Renderer, auf dem wir später Grafiken zeichnen.
	 * Ein Zeiger auf ein Fenster. So ein Standard-Fenster.
	 * Ein Event. Hier schreiben wir später einkommende Events rein und schauen,
	 * was das wohl für welche sein werden.
	 * */
	SDL_Renderer *renderer = nullptr;
	SDL_Window *window = nullptr;
	SDL_Event event;


	/*
	 * Wir brauchen ein Bild. Das nutzen wir um damit den Web abzulaufen, den
	 * die Gegner gehen sollen.
	 *
	 * Beim Laden eines Bildes erhalten wir zunächst ein Surface. Das wandeln
	 * wir anschließend in eine Texture. Diese lässt sich leicht auf unseren
	 * renderer zeichnen.
	 * Außerdem brauchen wir ein Rechteck. Dort packen wir dann x,y,w,h unseres
	 * Bildes rein.
	 * */
	SDL_Surface *surface = nullptr;
	SDL_Texture *textureEinheit = nullptr;
	SDL_Texture *textureTurm = nullptr;
	SDL_Rect rectEinheit{0,0,0,0};
	SDL_Rect rectTurm{0,0,0,0};

	// Eine Liste aller aktiven Einheiten
	// so können wir diese leichter überwachen
	std::vector<Einheit> aktiveEinheiten;
	std::vector<std::vector<Einheit>::iterator> verloreneEinheiten;


	// "Versuchen" wir doch einfach mal. Und falls ein Fehler/ eine Ausnahme
	// auftreten sollte, wird sie weiter unten gefangen.
	try{

		// SDL muss initialisiert werden. Das tuen wir am besten so früh wie
		// möglich. Sollte dies nicht klappen, schmeißen wir einen Fehler und
		// beenden alles.
		SDL_ANNAHME( SDL_Init( SDL_INIT_EVERYTHING) != -1);


		/*
		 * SDL_image müssen wir auch initialisieren.
		 * In dem Fall wollen wir gerne PNG nutzen. Ein paar andere Formate
		 * ewrden auch unterstützt, interessieren uns aber gerade nicht.
		 * */
		IMG_ANNAHME( (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != 0);

		/*
		 * Nun legen wir ein neues Fenster an.
		 * Die Parameter sprechen im Prinzip für sich.
		 * OpenGL als Flag sorgt dafür, dass wir eine beschleunigte Ausgabe
		 * bekommen. Wir sind dadurch nicht gezwungen die OpenGL Befehle zu
		 * nutzen.
		 * */
		window = SDL_CreateWindow(
				"Tower Defense Tutorial",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				1024, 
				768, 
				SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
				);

		// 'Eigentlich' sollten wir jetzt ein Fenster haben. Unsere
		// window-Variable darf also nicht mehr nullptr sein, sondern muss auf
		// ein Fenster zeigen. Falls nicht, FEHLER, und Ende.
		SDL_ANNAHME(window != nullptr);


		/*
		 * Mit dem frisch erzeugen Fenster können wir einen Renderer anlegen. 
		 * -1 sagt hier so viel wie: "Gibt mir den Besten er zu mir passt"
		 *  Die Flags vordern Hardwarebeschleunigung und VSync.
		 *  Hardwarebeschleunigung nutzt die Grafikkarte aus. Die CPU wird
		 *  entlastet. Die GPU ist sowieso viel besser in den grafischen
		 *  Aufgaben.
		 *  VSync sorgt dafür, dass wir genau so viel Bilder pro Sekunde
		 *  bekommen, wie unser Monitor kann.
		 *  Wir können auch VSync ausmachen und bekommen so viele viele Bilder
		 *  pro Sekunde. Aber das belastet die CPU und GPU nur unnötig.
		 * */
		renderer = SDL_CreateRenderer(
				window, 
				-1, 
				SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
				);

		// Wieder hoffen wir, dass die Operation erfolgreich war. Unsere
		// renderer Variable sollte nun auf einen Renderer zeigen.
		SDL_ANNAHME(renderer != nullptr);


		/*
		 * Laden wir also das Bild.
		 * Diesmal als PNG und mit SDL_image.
		 * */
		surface = IMG_Load("images/enemy.png");

		// Hoffentlich konnte das Bild geladen werden ...
		SDL_ANNAHME(surface != nullptr);

		// Aus dem surface, der "Fläche", die wir haben, erstellen wir die
		// Texture. Und wieder ein Check, dass dies auch geklappt hat.
		textureEinheit = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_ANNAHME(textureEinheit != nullptr);

		/*
		 * Das Bild beginnt bei 0,0 und ist w,h Pixel groß. Die Werte finden wir
		 * in surface.
		 * */
		rectEinheit.x = 0;
		rectEinheit.y = 0;
		rectEinheit.w = surface->w;
		rectEinheit.h = surface->h;

		// Das surface brauchen wir nicht mehr und können den Speicher
		// freigeben. Sicherheitshalber setzen wir es auch auf den nullptr.
		SDL_FreeSurface(surface);
		surface = nullptr;

        // Die Texture des Turmes
		surface = IMG_Load("images/turret.png");
		SDL_ANNAHME(surface != nullptr);
		textureTurm = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_ANNAHME(textureTurm != nullptr);
		rectTurm.x = (1024-32)/2; // Der Turm soll in der Mitte des Bildschirms
		rectTurm.y = (768-32)/2; // angezeigt werden
		rectTurm.w = surface->w;
		rectTurm.h = surface->h;


		// Wir erzeugen eine Wegpunktliste und bekommen davon einen shared_ptr
		auto alleWegpunkte = std::make_shared<WaypointList>();

		// Füllen wir ein paar Wegpunkte in die Liste
		alleWegpunkte->push_back({{0,0}});
		alleWegpunkte->push_back({{1024-32,0}});
		alleWegpunkte->push_back({{0,768-32}});
		alleWegpunkte->push_back({{1024-32,768-32}});
		alleWegpunkte->push_back({{0,0}});

		// Ein kleiner Gegner:
		Einheit einheit;
		einheit.init(textureEinheit, rectEinheit);
		einheit.setzeWegpunkte(alleWegpunkte);

		// die Einheit kommt in die Liste der aktiven Einheiten
		//
		// ! Es kommt nicht DIESE Einheit in die Liste.
		// Es wird eine Kopie davon erstellt!
		// ! "einheit" bleibt hier.
		// Am Besten mal ein kleines Stück Code dafür zur Demonstration.
		aktiveEinheiten.push_back(einheit);


		// Ein Türmchen
		Turm turm;
		turm.init(textureTurm, rectTurm);

		std::vector<std::array<int,4>> zuZeichnendeSchuesse;

		// Die aktuelle "Zeit" in Millisekunden
		// wir nutzen hier 'auto' als Typangabe. C++ weiß selber, was für ein
		// Typ das sein wird, weil dieser ja durch den Rückgabewert der
		// Funktion bestimmt ist. 
		// "auto irgendwas;" funktioniert nicht, da keine Typinformation gegeben
		// ist. Durch die direkte Zuweisung, wie unten, geht das.
		// 
		// Wir nutzen hier noch einen kleinen ZeitCounter, damit wir nach
		// 1000ms, also nach einer Sekunde anzeigen können, wie viel Frames
		// gerade gerendert wurden. Die FPS - frames pro sekunde.
		auto startZeit = SDL_GetTicks();
		auto endZeit = SDL_GetTicks();
		auto differenzZeit = endZeit - startZeit;
		int zeitCounter = 0;
		int framesProSekunde = 0;

		bool running = true;

		/*
		 * Die Hauptschleife.
		 * Das wichtigste Dinge.
		 * Sie läuft und läuft und läuft ...
		 * bis irgendwann die running Variable false ist.
		 * */
		while(running){
			startZeit = SDL_GetTicks();

			/*
			 * Die Event-Schleife.
			 * Wir schauen, ob ein Event da ist. Sollte eins da sein, wir es in
			 * unsere event-Variable geschrieben.
			 * */
			while( SDL_PollEvent(&event)){

				/*
				 * Haben wir ein Event, dann schauen wir, was das ist.
				 * Hier schauen wir erstmal nur nach Taste loslasen und 'Quit'.
				 * Beide male setzten wir running auf false. So wird unsere
				 * Hauptschleife später enden.
				 * */
				switch(event.type){
					case SDL_KEYUP:
					case SDL_QUIT:
						running = false;
						break;
				}
			}

			// Wir haben Events bekommen und können reagieren.
			// Also können wir hier unsere Einheiten updaten.
			// alle aktiven Einheiten werden geupdatet.
			for( auto &e:aktiveEinheiten) e.update(differenzZeit);
			//einheit.update(differenzZeit);
			turm.update(differenzZeit);



			// Wir müssen hier mit den Iteratoren hantieren, da wir nur so
			// wissen, welches Element wir nachher entfernen können.
			// std::vector erase nimmt einen Iterator
			// also müssen wir ihm diesen geben
			//
			// Warum entfernen wir die Einheit nicht gleich wenn wir wissen,
			// dass sie hinüber ist?
			// Tja.
			// Das liegt hier an der Datenstruktur.
			// Während wir über diesen Vector laufen, können wir keine Elemente
			// davon entfernen oder irgendwo einfügen. Wer das versucht, darf
			// sich auf Fehler einstellen.
			// Deswegen packen wir die Einheit erst einmal zusätzlich in die
			// extra Liste, und nehmen sie erst nachdem wir fertig sind, heraus.
			for( auto it = aktiveEinheiten.begin(); it < aktiveEinheiten.end(); ++it){
            //for( auto &e:aktiveEinheiten){
				if( turm.shoot(*it)){
					std::clog << "Treffer!" << std::endl;

                    // Der Turm hat geschossen. Das sollten wir auch anzeigen.
					// Am einfachsten mit einer Linie von Turm zu Einheit.
					// Zeichnen tun wir aber erst weiter unten, also speichern
					// wir die beiden Coordinaten und zeigen sie später an.
					// 
					// Problem: Der Schuss wird hier von Position aus geschickt.
					// Position ist aber oben links vom Bild/der Textur. Es
					// sieht etwas seltsam aus. Also wäre es etwas besser, wenn
					// wir von der Mitte des Turm aus schießen und auch die
					// Einheit in der Mitte treffen.
					// Trick 17: Wir wissen, dass die Bilder 32px breit und hoch
					// sind. Die Hälfte ist die Mitte, also bei 16. Vom Rand
					// gehen wir also einfach 16 schritte runter und rüber und
					// sind in der Mitte.
					// FIXME: Setzte Mitte anhand der Bildgröße und nicht nach
					// "Wissen"
					zuZeichnendeSchuesse.push_back({{
							turm.getPosition()[0] + 16, turm.getPosition()[1] + 16,
							it->getPosition()[0] + 16, it->getPosition()[1] + 16}});

					if( it->gotHit(1)){ // FIXME: setzte Schadenswert vom Turm
						std::clog << "Versenkt!" << std::endl;
						// jetzt ists vorbei mit Einheit e
						// deswegen kommt die Einheit in eine Liste
						// die Liste der frisch Verstorbenen 
						verloreneEinheiten.push_back(it);
					}
				}
			//}
			}

			// jede verlorene Einheit wird jetzt von den aktiven Einheiten
			// entfernt
			for( auto it:verloreneEinheiten){
				aktiveEinheiten.erase(it);
				aktiveEinheiten.push_back(einheit); // just mal aus Spaß
			}

			// wir haben alle verlorene Einheiten von den aktiven entfernt
			// jetzt können wir diese auch aus dieser Liste heraus nehmen
			verloreneEinheiten.clear();

			/*
			 * Hier unten zeichnen wir auf unseren renderer
			 * Noch passiert nicht viel.
			 * Wir löschen die Fläche 'clear'
			 * und zeigen dann die Fläche an 'present'
			 * Dabei nutzen wir das Prinzip des DoubleBuffering.
			 * Es gibt 2 Buffer.
			 * Einer davon wird angezeigt, auf den anderen schreiben wir in der
			 * Zeit. Mit 'RenderPresent' tauschen wir die beiden Buffer aus. Wo
			 * wir drauf gemalt haben, der geht nach Vorne und wird sichtbar.
			 * Der andere kommt zu uns nach Hinten und wir können darauf
			 * herummalen.
			 * */
			SDL_RenderClear(renderer);

            // Eigentlich können wir hier mal unser Bildchen zeichnen.
			// Wir kopieren dazu unsere Texture auf unsere Render-Fläche, den
			// renderer. nullptr heißt hier, dass die gesamte Texture kopiert
			// werden soll. Wir könnten aber auch nur einen Teil davon kopieren.
			// Der letzte Parameter, rect, gibt das Ziel an. Bisher ist es noch
			// 0,0,w,h und damit müsste das Bild oben links in der Ecke
			// auftauchen.
            //SDL_RenderCopy(renderer, textureEinheit, nullptr, &rect);

            // Und hier zeichnen wir die Einheiten
			// alle aktiven Einheiten auf den Renderer zeichnen
			for( auto &e:aktiveEinheiten) e.draw(renderer);
			//einheit.draw(renderer);
			turm.draw(renderer);

            // Schüsse zeichnen
			//
			// Da wir die aber nur ein Frame lang zeichnen, könnte es sein, dass
			// wir davon nicht viel mitbekommen. Werden wir ja im Test sehen ;-)
			//
			// Die Linie sollte eine Farbe != schwarz haben. Aber vorher
			// speichern wir die aktuelle Farbe und setzen diese anschließend
			// auch wieder. Man kann ja nicht wissen, was andere zuvor
			// angestellt haben...
			Uint8 rgba[4];
			SDL_GetRenderDrawColor(renderer, &rgba[0], &rgba[1], &rgba[2], &rgba[3]);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			for( auto &line:zuZeichnendeSchuesse){
				SDL_RenderDrawLine(renderer, line[0], line[1], line[2], line[3]);
			}
			zuZeichnendeSchuesse.clear();
			SDL_SetRenderDrawColor(renderer, rgba[0], rgba[1], rgba[2], rgba[3]);


			SDL_RenderPresent(renderer);


			// Die Zeit für ein Frame bekommen wir, in dem wir die Zeit am Ende
			// minus der Zeit am Anfang rechnen.
            endZeit = SDL_GetTicks();
			differenzZeit = endZeit - startZeit;
			zeitCounter += differenzZeit;

			// Wir haben wieder einen Frame geschafft.
			// Haben wir bereits 1000ms hinter uns, dann geben wir auf der
			// Konsole auf dem speziellen Log-Stream eine Nachricht aus, die die
			// FPS anzeigt.
			++framesProSekunde;
			if( zeitCounter >= 1000){
				std::clog << "[INFO] FPS: " << framesProSekunde << std::endl;
				framesProSekunde = 0;
				zeitCounter = 0;
			}
		}

	
	// Fangen wir Exceptions!
	// In dem Fall fangen wir eine Ausnahme vom Type std::runtime_error
	// Irgendwo werden wir diese wohl geworfen haben ...
	}catch(std::runtime_error re){
		// Sicherheitshalber geben wird den Fehler auf der Ausgabe für Fehler
		// aus. CERR steht für Console ERRor.
		std::cerr << re.what() << std::endl;
	}

	/*
	 * Wir müssen noch etwas aufräumen.
	 * Wahrscheinlich haben wir einen renderer und ein window. Die müssen noch
	 * zerstört werden.
	 * Auch die Texture, die wir wahrscheinlich haben, muss freigegeben werden.
	 * */
	SDL_DestroyTexture(textureTurm);
	SDL_DestroyTexture(textureEinheit);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);


	// Und am Ende natürlich ganz sicher gehen und SDL beenden.
	// Und SDL_image auch.
	IMG_Quit();
	SDL_Quit();


	/* Das Ende.
	 * Im Normalfall - und wenn alles OK war, geben wir 0 aus.
	 * */
	return 0;
}
