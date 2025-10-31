// ============================================================================
// PROGRAMME D’AUTHENTIFICATION Coffre-fort
// Description : Ce programme vérifie l’identité d’un agent qui connect un circuit électronique à Arduino
// analogiques et de plusieurs méthodes d’authentification logiques (MA1 à MA5).
// ============================================================================

// ---------------------------
// DÉFINITION DES CONSTANTES
// ---------------------------

// Tolérances pour la comparaison des lectures analogiques
const float tol1 = 0.90;  // Tolérance inférieure (90% de la valeur de référence)
const float tol2 = 1.10;  // Tolérance supérieure (110% de la valeur de référence)

// Tableau de lettres pour identifier les agents (16 agents max)
const char x[16] = {'A','B','C','D','E','F','H','I','J','K','L','M','N','O','P','Q'};

// Tableau des exposants publics pour le chiffrement (RSA simplifié)
const int e[16] = {601,619,631,641,647,653,661,673,691,701,733,739,751,797,809,811};

// Tableau de valeurs complémentaires (non utilisé ici, potentiellement pour un futur module)
const int d[16] = {1273,103,883,653,467,641,1321,589,1039,953,745,919,1159,713,233,1063};

// ---------------------------
// CONFIGURATION INITIALE
// ---------------------------

void setup() {
  // Initialise la communication série à 9600 bauds pour la communication avec le PC
  Serial.begin(9600);
  
  // Initialise le générateur de nombres aléatoires avec un seed analogique
  // (permet d'obtenir des valeurs pseudo-aléatoires plus variées)
  randomSeed(analogRead(0));
}

// ---------------------------
// BOUCLE PRINCIPALE
// ---------------------------

void loop() {
  // Lecture de la résistance équivalente via l’entrée analogique A0
  int resistance = analogRead(A0);
  
  // Conversion de la valeur brute (0-1023) en tension (0-5V)
  float res_eq = (float)5 - resistance * (5.0 / 1023.0);
  
  // Affichage de la tension mesurée sur le moniteur série
  Serial.println(res_eq);
  
  // Tableau de points de contrôle pour différents modèles (seuils de comparaison)
  const float checkPoints[] = {5, 1.52, 1.91, 2.25, 2.45, 2.73, 3.09, 3.39};
  
  // Parcourt tous les points de contrôle pour déterminer le modèle détecté
  for (int i = 0; i < 8; i++) {
    // Vérifie si la lecture est dans la plage de tolérance du point courant
    if (res_eq > checkPoints[i] * tol1 && res_eq < checkPoints[i] * tol2) {
      
      // Sélection du modèle correspondant à l’index trouvé
      switch (i) {
        case 0:
          Serial.println("Modèle 1");
          if (securite1()) Serial.println("Authentification réussie, Bravo !!!");
          else Serial.println("Authentification interrompue, la sécurité est en chemin !!!");
          break;

        case 1:
          Serial.println("Modèle 2");
          if (securite2()) Serial.println("Authentification réussie, Bravo !!!");
          else Serial.println("Authentification interrompue, la sécurité est en chemin !!!");
          break;

        case 2:
        case 3:
          Serial.println(i == 2 ? "Modèle 3" : "Modèle 4");
          if (securite3()) Serial.println("Authentification réussie, Bravo !!!");
          else Serial.println("Authentification interrompue, la sécurité est en chemin !!!");
          break;

        case 4:
          Serial.println("Modèle 5");
          if (securite4()) Serial.println("Authentification réussie, Bravo !!!");
          else Serial.println("Authentification interrompue, la sécurité est en chemin !!!");
          break;

        case 5:
          Serial.println("Modèle 6");
          if (securite5()) Serial.println("Authentification réussie, Bravo !!!");
          else Serial.println("Authentification interrompue, la sécurité est en chemin !!!");
          break;

        case 6:
          Serial.println("Modèle 7");
          if (securite1()) Serial.println("Authentification réussie, Bravo !!!");
          else Serial.println("Authentification interrompue, la sécurité est en chemin !!!");
          break;

        case 7:
          Serial.println("Modèle 8");
          if (securite2()) Serial.println("Authentification réussie, Bravo !!!");
          else Serial.println("Authentification interrompue, la sécurité est en chemin !!!");
          break;
      }

      // Délai d’attente avant reprise de la boucle (10 secondes)
      delay(10000);
      return; // Sortie de la boucle loop()
    }
  }
}

// ============================================================================
// MÉTHODES DE SÉCURITÉ (combinaisons de MAx)
// ============================================================================

bool securite1() { return MA1() && MA2(); }                   // MA1 + MA2
bool securite2() { return MA1() && MA4(); }                   // MA1 + MA4
bool securite3() { return MA2() && MA5(); }                   // MA2 + MA5
bool securite4() { return MA2() && MA3() && MA4(); }          // MA2 + MA3 + MA4
bool securite5() { return MA1() && MA2() && MA3() && MA5(); } // MA1 + MA2 + MA3 + MA5

// ============================================================================
// MÉTHODE D’AUTHENTIFICATION MA1 : QCM DE QUESTIONS
// ============================================================================

bool MA1() {
  Serial.println("Question 1 : Nom de l'agence ?");
  Serial.println("1 : MI7 ; 2 : FBI; 3 : M007 ; 4 : Totally Spies ");
  while (Serial.available() <= 0);
  int Reponse = Serial.parseInt();

  if (Reponse != 1) return false; // Bonne réponse = MI7

  Serial.println("Question 2 : Nom de l'agent responsable ?");
  Serial.println("1 : L'agent P; 2 : L'agent M; 3 : L'agent R; 4 : L'agent B");
  while (Serial.available() <= 0);
  Reponse = Serial.parseInt();
  if (Reponse != 3) return false; // Bonne réponse = R

  Serial.println("Question 3 : Nom du directeur ?");
  Serial.println("1 : L'agent Musk; 2 : L'agent Z; 3 : L'agent N; 4 : Didier Deschamps");
  while (Serial.available() <= 0);
  Reponse = Serial.parseInt();
  return (Reponse == 3); // Bonne réponse = N
}

// ============================================================================
// MÉTHODE D’AUTHENTIFICATION MA2 : IDENTIFICATION + CHIFFREMENT RSA
// ============================================================================

bool MA2() {
  // Nettoie le buffer
  while (Serial.available() > 0) Serial.read();
  
  Serial.println("Donnez votre identite :");
  while (Serial.available() <= 0);
  char yinput = Serial.read();

  // Recherche de la lettre dans la table d'agents
  int index = -1;
  for (int i = 0; i < 16; ++i) {
    if (yinput == x[i]) index = i;
  }
  if (index == -1) {
    Serial.println("Identite invalide");
    return false;
  }

  // Génère un message aléatoire et le chiffre
  int M = random(2882);
  int C = chiffrer(M, e[index]);
  Serial.println("Message a dechiffrer : " + String(C));
  Serial.println(M); // (à masquer en version finale)

  // Lecture de la réponse
  while (Serial.available() <= 0);
  delay(100);
  String input = "";
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (isdigit(c)) input += c;
  }
  
  int Mp = input.toInt();
  if (M == Mp) {
    Serial.println("Vous êtes authentifié! Bienvenue agent " + String(x[index]));
    return true;
  } else {
    Serial.println("Erreur d'authentification, le système de sécurité va se déclencher.");
    return false;
  }
}

// Fonction de chiffrement RSA simplifié
int chiffrer(int M, int e) {
  int n = 43 * 67; // Module RSA (produit de deux nombres premiers)
  return modeExp(M, e, n);
}

// Exponentiation modulaire : calcule (a^e) mod n
int modeExp(int a, int e, int n) {
  long long r = 1;
  if (e < 0) {
    Serial.println("cas invalide");
    return -1;
  }
  while (e > 0) {
    r = (r * (a % n)) % n;
    e--;
  }
  return r;
}

// ============================================================================
// MÉTHODES D’AUTHENTIFICATION MA3 À MA5
// ============================================================================

// MA3 : Vérifie la couleur des yeux
bool MA3() {
  while (Serial.available() > 0) Serial.read();
  Serial.println("Quelle est la couleur de mes yeux : ");
  while (Serial.available() <= 0);
  char couleur = Serial.read();
  return (couleur == 'b'); // 'b' pour bleu
}

// MA4 : Vérifie quel doigt est le plus long
bool MA4() {
  while (Serial.available() > 0) Serial.read();
  Serial.println("Quel est le doigt le plus long : ");
  while (Serial.available() <= 0);
  char doigt = Serial.read();
  return (doigt == 'm'); // 'm' pour majeur
}

// MA5 : Vérifie la correspondance d’un Card ID binaire et d’une lettre
bool MA5() {
  while (Serial.available() > 0) Serial.read();
  Serial.println("Entrer votre Card Id : ");
  while (Serial.available() <= 0);
  int CardId = Serial.parseInt();

  // Conversion binaire -> décimal
  float decimal = 0;
  int i = 0;
  int tempCardId = CardId;
  while (tempCardId != 0) {
    int dec = tempCardId % 10;
    decimal = decimal + dec * pow(2, i);
    tempCardId = tempCardId / 10;
    i++;
  }

  // Vérifie la lettre associée
  Serial.println("Entrer votre nom :");
  while (Serial.available() <= 0);
  char LettreAgent = Serial.read();
  
  return (decimal >= 0 && decimal < 16 && LettreAgent == x[(int)decimal]);
}
