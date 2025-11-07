# C++ Projekt: Bibliothek-Verwaltungssystem

## Projekt-Kontext

Dies ist mein Abschlussprojekt fuer das Wahlmodul "C++ Fortschritte" an der Hochschule Niederrhein. Die Modulnote basierte ausschliesslich auf diesem Projekt.

Es handelt sich um eine vollstaendige, grafische Desktop-Anwendung zur Verwaltung einer Bibliothek.

## Verwendete Technologien

* **Kern-Logik:** C++
* **UI-Framework:** Qt
* **UI-Aufbau:** Die gesamte Benutzeroberflaeche wurde prozedural in C++ erstellt (ohne den Qt Designer).
* **Styling:** QSS (Qt Style Sheets) zur visuellen Gestaltung der Komponenten.

## Kern-Features

Die Anwendung verfuegt ueber zwei separate Rollen mit unterschiedlichen Zugaengen und Rechten: **Benutzer** (Ausleiher) und **Bibliothekar** (Admin).

### Features (Benutzer-Ansicht)

* **Rollenbasierter Login:** Getrennte Anmeldebildschirme fuer Benutzer und Bibliothekare.
* **Buecher-Katalog:** Eine Uebersicht aller verfuegbaren Buecher mit Cover, Beschreibung und Suchfunktion.
* **Ausleih-Anfragen:** Benutzer koennen Anfragen stellen, um Buecher auszuleihen.
* **"Meine Ausleihen":** Ein persoenliches Dashboard, das alle aktuellen und vergangenen Ausleihvorgaenge anzeigt (Status: Ausgeliehen, Zurueckgegeben, Abgelehnt).
* **Aktionen:** Benutzer koennen Buecher zurueckgeben oder Verlaengerungen anfragen.

### Features (Bibliothekar-Ansicht / Admin-Panel)

* **Buecher-Katalog (Admin):** Uebersicht mit erweiterten Optionen (Bearbeiten/Loeschen).
* **Buch-Management:** Ein CRUD-Interface (Create, Read, Update, Delete) zum Hinzufuegen von Buechern.
* **Ausleih-Management:** Ein Dashboard zur Verwaltung aller Ausleihanfragen (Genehmigen oder Ablehnen).
* **Rueckgabe-Management:** Eine Uebersicht aller zurueckgegebenen Buecher.
* **Statistik-Dashboard:** Eine grafische Uebersicht (Kuchendiagramm) ueber den Status aller Buecher.

## Screenshots der Anwendung

*(Die folgenden Screenshots zeigen die verschiedenen Ansichten und Funktionen der fertigen Anwendung.)*

---

**1. Login & Rollen-Auswahl**
![Login-Bildschirm der Bibliothek-Anwendung](screenshot/1.jpg)
![Alternativer Login-Bildschirm](screenshot/2.jpg)

---

**2. Buecher-Uebersicht (Katalog fuer Benutzer)**
![Uebersicht des Buch-Katalogs](screenshot/3.jpg)

---

**3. "Meine Ausleihen" (Benutzer-Dashboard)**
![Dashboard fuer Benutzer-Ausleihen](screenshot/4.jpg)

---

**4. Buecher-Uebersicht (Bibliothekar-Ansicht)**
![Uebersicht des Buch-Katalogs aus Admin-Sicht](screenshot/5.jpg)

---

**5. Buch Hinzufuegen (Bibliothekar-Funktion)**
![Formular zum Hinzufuegen eines neuen Buches](screenshot/6.jpg)

---

**6. Ausleih-Management (Bibliothekar-Dashboard)**
![Dashboard zur Verwaltung von Ausleihanfragen](screenshot/7.jpg)
![Dashboard zur Verwaltung von Rueckgaben](screenshot/8.jpg)

---

**7. Statistik-Ansicht (Bibliothekar-Funktion)**
![Statistik-Seite mit Kuchendiagramm](screenshot/9.jpg)
