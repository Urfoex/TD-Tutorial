/* Starten wir mit dem einfachsten.
 * 
 * Wir brauchen die Ausgabe auf die Konsole und SDL2.
 * Also includen wir diese erstmal hier.
 * */
#include <iostream>
/*
 * Hier unter Linux liegt SDL.h in /usr/include/SDL2/
 * Über CMake wird dieser Pfad automatisch mit als Include-Pfad an den Kompiler
 * übergeben.
 * */ 
#include <SDL.h>
/*
 * Falls Fehler auftreten, werfen wir einfach mit exceptions um uns. Irgendwer
 * wird sie schon fangen. Hier in den Includes sind ein paar Ausnahmen
 * definiert, die wir später einfach nutzen werden.
 * */
#include <exception>
#include <stdexcept>

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

	// "Versuchen" wir doch einfach mal. Und falls ein Fehler/ eine Ausnahme
	// auftreten sollte, wird sie weiter unten gefangen.
	try{

		// SDL muss initialisiert werden. Das tuen wir am besten so früh wie
		// möglich. Sollte dies nicht klappen, schmeißen wir einen Fehler und
		// beenden alles.
		SDL_ANNAHME( SDL_Init( SDL_INIT_EVERYTHING) != -1);

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

		bool running = true;

		/*
		 * Die Hauptschleife.
		 * Das wichtigste Dinge.
		 * Sie läuft und läuft und läuft ...
		 * bis irgendwann die running Variable false ist.
		 * */
		while(running){

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
			SDL_RenderPresent(renderer);

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
	 * */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);


	// Und am Ende natürlich ganz sicher gehen und SDL beenden.
	SDL_Quit();


	/* Das Ende.
	 * Im Normalfall - und wenn alles OK war, geben wir 0 aus.
	 * */
	return 0;
}
