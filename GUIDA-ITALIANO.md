# Pharaoh's Ascent: guida Windows 11 e Full HD

Questa patch non ufficiale richiede **l'edizione originale inglese su ISO per Windows**. Il gioco non è incluso. Corregge l'avvio sul PC Windows 11 provato e aggiunge una visualizzazione proporzionata: nessuna immagine allargata per riempire lo schermo.

## 1. Recuperare il gioco corretto

1. Apri [Pharaoh's Ascent su My Abandonware](https://www.myabandonware.com/game/pharaoh-s-ascent-rgo).
2. Scorri fino a **Download Pharaoh's Ascent**, sezione **Windows Version**.
3. Scegli **ISO Version**, indicata come circa **147 MB** al momento della verifica.
4. Il download usato per i test si chiama **`Pharaoh-s-Ascent_Win_EN_ISO-Version.zip`** e occupa 154.341.818 byte.
5. Al suo interno si trova **`Pharaohs Ascent.iso`**, di 182.626.304 byte.
6. Conserva lo ZIP: puoi darlo direttamente all'installatore della patch. Non serve montare l'ISO, installare Windows XP o avviare il vecchio setup.

Edizioni tedesche, demo, repack, installazioni già modificate e altre versioni non sono supportate. L'installatore verifica il contenuto originale di 194 file: rinominare un archivio diverso non lo rende compatibile. Recupera e utilizza il gioco nel rispetto dei diritti applicabili alla tua copia.

Per identificare esattamente il download provato, questi sono gli hash SHA-256:

| File | SHA-256 |
|---|---|
| ZIP del gioco | `511e70a7195a1e443f2079602a3b44fffc409945898b8c6b0bfb8dd802ae12c4` |
| ISO | `b80a04ab3fd199966be5917a411e5d9433ffe1c53a3a4477ce83b600681c60e7` |

Verifica facoltativa in PowerShell: `Get-FileHash -Algorithm SHA256 "C:\percorso\Pharaohs Ascent.iso"`. Se il sito ricomprime lo ZIP, il suo hash può cambiare: la compatibilità viene comunque decisa dal controllo dei file originali all'interno.

## 2. Scaricare la patch

1. Vai alla **[release più recente](https://github.com/jshodanVR/pharaohs-ascent-windows11-patch/releases/latest)**.
2. Espandi **Assets**, se necessario.
3. Scarica **`Pharaoh-Ascent-W11-Patch-1.0.zip`**, circa 32 KB. Non confonderlo con lo ZIP del gioco da 147 MB.
4. Clic destro sullo ZIP della patch → **Estrai tutto**.
5. Apri la cartella estratta: devono esserci `INSTALL.cmd`, `Install.ps1`, `patch.json`, `PharaohHD.dll` e la cartella `src`.

Scarica il file indicato negli Assets, anziché gli archivi automatici **Source code (zip)** di GitHub. Mantieni insieme tutti i file. Non avviare l'installatore dentro l'anteprima dello ZIP.

## 3. Installare e giocare

1. Fai doppio clic su **`INSTALL.cmd`**.
2. Nella finestra di selezione scegli **lo ZIP originale del gioco**, oppure l'ISO estratta. Non scegliere lo ZIP della patch.
3. Scegli una cartella padre in cui puoi scrivere, ad esempio Documenti o Giochi.
4. Al suo interno verrà creata **`Pharaohs-Ascent-Windows11`**. Se esiste già, scegli un'altra cartella padre: la patch non sovrascrive giochi o salvataggi esistenti.
5. Attendi il messaggio di installazione completata. I file vengono verificati, copiati e corretti automaticamente.
6. Apri la nuova cartella del gioco e avvia **`PLAY.cmd`**.

Non servono privilegi amministratore, Python, programmi di estrazione aggiuntivi, codec o modifiche alle DLL di Windows. L'installatore usa Windows PowerShell 5.1 ed `expand.exe`, presenti su Windows 11. L'opzione di esecuzione PowerShell vale solo per il processo avviato e non modifica l'impostazione globale.

Lo ZIP e l'ISO originali rimangono intatti. Usa sempre `PLAY.cmd`: il parametro `/NV` evita i vecchi filmati, che questa versione della patch **non corregge**. Non riattivarli nelle preferenze del gioco.

## 4. Full HD senza deformazioni

La finestra iniziale è senza bordi e misura 1920×1080. L'immagine originale 1024×768 viene ingrandita proporzionalmente a 1440×1080, con bande nere laterali di 240 pixel. Le bande sono volute: preservano le proporzioni. La grafica è ingrandita, non ridisegnata in alta definizione.

- **F11** oppure **Alt+Invio**: passa a schermo intero alla risoluzione corrente del monitor.
- **F10**: riduce a icona.
- Su schermi più piccoli, l'uscita viene limitata allo spazio disponibile.

Per modificare l'avvio, chiudi il gioco e apri `PharaohHD.ini` nella nuova cartella. Le opzioni sono `Width=1920`, `Height=1080` e `Fullscreen=0`. Imposta `Fullscreen=1` per partire a schermo intero. Lascia disattivato il debug durante il normale utilizzo.

## 5. Comandi

| Azione | Comando |
|---|---|
| Entrare nella stanza selezionata | Invio |
| Muoversi | Frecce o tastierino numerico; Bloc Num disattivato |
| Saltare | Spazio + direzione |
| Sparare | Ctrl + direzione |
| Abbassarsi / rialzarsi | Giù + Spazio / Su |
| Ricominciare la stanza | Ctrl+Z |
| Tornare alla piramide | Ctrl+X; da lì scegli Exit per uscire |

Porta il puntatore al bordo superiore per mostrare il menu. Se necessario scegli **Controller → Keyboard**. Per aprire l'uscita, spingi la pietra rotonda con l'ankh nella pietra con la serratura.

## 6. Problemi comuni

| Problema | Soluzione |
|---|---|
| Destination already exists | Scegli un'altra cartella padre. Non è previsto aggiornare un'installazione esistente. |
| Unsupported or damaged game file | Usa l'ISO inglese indicata; riscaricala se incompleta. Non mescolare edizioni o aggirare la verifica. |
| File della patch mancanti / DLL danneggiata | Estrai nuovamente tutta la patch, inclusa `src`. |
| Il gioco non parte | Avvia `PLAY.cmd` dalla cartella nuova, dopo il messaggio di installazione riuscita. |
| Il personaggio non si muove | Seleziona Controller → Keyboard, disattiva Bloc Num, clicca sul gioco e tieni premuta brevemente una freccia. |
| Filmati non funzionanti | Usa `PLAY.cmd` e mantienili disabilitati: non sono corretti in questa release. |
| Guida .HLP non apribile | Il file originale è conservato, ma non viene fornito un lettore WinHelp. |

Per segnalare un problema, apri una [Issue](https://github.com/jshodanVR/pharaohs-ascent-windows11-patch/issues) indicando errore esatto e versione di Windows. Puoi allegare `PATCH-INSTALL-REPORT.json`, se presente. Non caricare ISO, gioco, salvataggi personali o informazioni private.

## 7. Salvataggi, disinstallazione e limiti

Fai una copia della cartella `saved` prima di rimuovere o sostituire il gioco. La patch non include né trasferisce automaticamente salvataggi. Per disinstallare: chiudi il gioco, conserva i salvataggi necessari, poi elimina soltanto la nuova cartella del gioco e quella della patch estratta. Il download originale resta invariato. Rimane il comportamento originale delle preferenze e dei salvataggi; non viene garantito un funzionamento senza registro di sistema.

Provata su un PC Windows 11 x64: avvio, selezione stanze, prime stanze, sparo, movimento confermato dal giocatore e proporzioni dello schermo. La campagna completa, altri PC, Windows ARM, joystick e configurazioni con più monitor non sono stati verificati esaustivamente.

La distribuzione contiene solo adattatore, dati della patch, installatore, sorgenti e documentazione. Non contiene il gioco completo, immagini, audio, filmati o salvataggi. Il progetto non è affiliato al produttore originale, Microsoft o My Abandonware; i diritti sul gioco rimangono ai rispettivi titolari.
