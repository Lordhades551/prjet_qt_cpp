#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <utility> // Pour std::move
#include <QRegularExpression>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QTextDocument>
#include <QPageSize>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextTable>
#include <QTextCursor>
#include <QTextTableFormat>





//Constructeur de MainWindow
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
      // Configuration du modèle et de la vue pour la table des employés
        model = new QSqlTableModel(this);
        model->setTable("employes");
        model->select();  // Load data from the "employes" table

        ui->employesTreeView->setModel(model);  // Attach the model to the QTreeView
        ui->employesTreeView->resizeColumnToContents(0);  // Adjust the first column


            connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::mettreAJourAffichage);
            connect(ui->supprimerButton, &QPushButton::clicked, this, &MainWindow::supprimerEmploye);
            connect(ui->modifierButton, &QPushButton::clicked, this, &MainWindow::modifierEmploye);
            connect(ui->rechercheButton, &QPushButton::clicked, this, &MainWindow::rechercherEmployeParNom);
            connect(ui->recherchebutton, &QPushButton::clicked, this, &MainWindow::rechercherEmployeParPoste);
            connect(ui->triAncienneteButton, &QPushButton::clicked, this, &MainWindow::trierEmployesParAnciennete);
            //connect(ui->buttonPDF, &QPushButton::clicked, this, &MainWindow::exporterPDF);









    // Connexion et création de la base de données SQLite
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("employes.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données.");
        return;
    }

    // Création de la table 'employes' si elle n'existe pas
    QSqlQuery query;
    query.exec("DROP TABLE IF EXISTS employes");
    query.exec("CREATE TABLE employes ("
               "id_employe INTEGER PRIMARY KEY, "
               "nom TEXT, "
               "prenom TEXT, "
               "email TEXT, "
               "poste TEXT, "
               "adresse TEXT, "
               "etat_civil TEXT, "
               "nombre_enfants INTEGER, "
               "numero_telephone TEXT, "
               "date_embauche TEXT)");

    // Connexions des boutons aux fonctions
    connect(ui->ajouterButton, &QPushButton::clicked, this, &MainWindow::ajouterEmploye);
    connect(ui->supprimerButton, &QPushButton::clicked, this, &MainWindow::supprimerEmploye);

    // Chargement initial des données
    mettreAJourAffichage();
}
void MainWindow::modifierEmploye()
{
    // Récupérer l'ID de l'employé à modifier
    QString idText = ui->idModifLineEdit->text();

    // Vérifier si le champ ID est vide
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un identifiant.");
        return;
    }

    int id = idText.toInt();

    // Vérifier si l'employé avec cet ID existe dans la base de données
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM employes WHERE id_employe = :id");
    checkQuery.bindValue(":id", id);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        QMessageBox::warning(this, "Erreur", "L'identifiant spécifié n'existe pas.");
        return;
    }

    // Récupération des informations à modifier à partir des champs de saisie
    QString nom = ui->nomLineEdit->text();
    QString prenom = ui->prenomLineEdit->text();
    QString email = ui->emailLineEdit->text();
    QString poste = ui->posteLineEdit->text();
    QString adresse = ui->adresseLineEdit->text();
    QString etatCivil = ui->etatCivilLineEdit->text();
    int nombreEnfants = ui->nombreEnfantsLineEdit->text().toInt();
    QString numeroTelephone = ui->numeroTelephoneLineEdit->text();
    QString dateEmbauche = ui->dateEmbaucheLineEdit->text();

    // Préparer la requête de modification
    QSqlQuery query;
    query.prepare("UPDATE employes SET nom = :nom, prenom = :prenom, email = :email, "
                  "poste = :poste, adresse = :adresse, etat_civil = :etatCivil, "
                  "nombre_enfants = :nombreEnfants, numero_telephone = :numeroTelephone, "
                  "date_embauche = :dateEmbauche WHERE id_employe = :id");

    // Lier les valeurs aux paramètres de la requête
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":adresse", adresse);
    query.bindValue(":etatCivil", etatCivil);
    query.bindValue(":nombreEnfants", nombreEnfants);
    query.bindValue(":numeroTelephone", numeroTelephone);
    query.bindValue(":dateEmbauche", dateEmbauche);
    query.bindValue(":id", id);

    // Exécuter la requête et vérifier si elle a réussi
    if (query.exec()) {
        QMessageBox::information(this, "Modification", "Employé modifié avec succès.");
        mettreAJourAffichage();  // Rafraîchir l'affichage pour refléter les modifications
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification de l'employé : " + query.lastError().text());
    }
}





    void MainWindow::actualiserEmployes() {
    model->select();  // Recharge les données depuis la base de données
    ui->employesTreeView->resizeColumnToContents(0);  // Ajuste les colonnes si nécessaire
}


    void MainWindow::ajouterEmploye()
    {
        // Récupération des valeurs des champs de l'interface utilisateur
        QString idText = ui->idLineEdit->text();
        QString nom = ui->nomLineEdit->text();
        QString prenom = ui->prenomLineEdit->text();
        QString email = ui->emailLineEdit->text();
        QString poste = ui->posteLineEdit->text();
        QString adresse = ui->adresseLineEdit->text();
        QString etatCivil = ui->etatCivilLineEdit->text();
        QString nombreEnfantsText = ui->nombreEnfantsLineEdit->text();
        QString numeroTelephone = ui->numeroTelephoneLineEdit->text();
        QString dateEmbauche = ui->dateEmbaucheLineEdit->text();

        // Vérification que tous les champs requis sont remplis
        if (idText.isEmpty() || nom.isEmpty() || prenom.isEmpty() || email.isEmpty() || poste.isEmpty() ||
            adresse.isEmpty() || etatCivil.isEmpty() || nombreEnfantsText.isEmpty() || numeroTelephone.isEmpty() ||
            dateEmbauche.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
            return;
        }

        // Vérification que les champs numériques sont bien des entiers
        bool idValid, nombreEnfantsValid;
        int id_employe = idText.toInt(&idValid);
        int nombre_enfants = nombreEnfantsText.toInt(&nombreEnfantsValid);

        if (!idValid) {
            QMessageBox::warning(this, "Erreur", "L'identifiant doit être un nombre entier.");
            return;
        }
        if (!nombreEnfantsValid) {
            QMessageBox::warning(this, "Erreur", "Le nombre d'enfants doit être un nombre entier.");
            return;
        }

        // Contrôle du nombre d'enfants ne dépassant pas 10
        if (nombre_enfants > 10) {
            QMessageBox::warning(this, "Erreur", "Le nombre d'enfants ne peut pas dépasser 10.");
            return;
        }

        // Vérification du format de l'email
        if (!email.contains("@") || !email.contains(".")) {
            QMessageBox::warning(this, "Erreur", "L'email doit contenir un symbole '@' et un point '.'.");
            return;
        }

        // Vérification que le numéro de téléphone contient exactement 8 chiffres
        QRegularExpression regex("^\\d{8}$");  // Regex pour vérifier exactement 8 chiffres
        if (!regex.match(numeroTelephone).hasMatch()) {
            QMessageBox::warning(this, "Erreur", "Le numéro de téléphone doit contenir exactement 8 chiffres.");
            return;
        }

        // Création de la requête pour insérer un employé dans la table employes
        QSqlQuery query;
        query.prepare("INSERT INTO employes (id_employe, nom, prenom, email, poste, adresse, etat_civil, nombre_enfants, "
                      "numero_telephone, date_embauche) "
                      "VALUES (:id_employe, :nom, :prenom, :email, :poste, :adresse, :etat_civil, :nombre_enfants, "
                      ":numero_telephone, :date_embauche)");

        // Liaison des valeurs à la requête
        query.bindValue(":id_employe", id_employe);
        query.bindValue(":nom", nom);
        query.bindValue(":prenom", prenom);
        query.bindValue(":email", email);
        query.bindValue(":poste", poste);
        query.bindValue(":adresse", adresse);
        query.bindValue(":etat_civil", etatCivil);
        query.bindValue(":nombre_enfants", nombre_enfants);
        query.bindValue(":numero_telephone", numeroTelephone);
        query.bindValue(":date_embauche", dateEmbauche);

        // Exécution de la requête
        if (query.exec()) {
            // Si l'ajout est réussi, affiche un message de succès
            QMessageBox::information(this, "Succès", "Employé ajouté avec succès");
            mettreAJourAffichage();  // Mettre à jour l'affichage
        } else {
            // Si l'ajout échoue, affiche un message d'erreur avec le texte de l'erreur
            QMessageBox::critical(this, "Erreur", "Échec de l'ajout de l'employé : " + query.lastError().text());
        }
    }


void MainWindow::supprimerEmploye()
{
    // Récupère l'ID de l'employé à supprimer
    QString idText = ui->idASupprimerLineEdit->text();

    // Vérifie si le champ est vide
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un identifiant.");
        return;
    }

    int id = idText.toInt();

    // Vérifie si l'employé avec cet ID existe dans la base de données
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM employes WHERE id_employe = :id");
    checkQuery.bindValue(":id", id);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        QMessageBox::warning(this, "Erreur", "L'identifiant spécifié n'existe pas.");
        return;
    }

    // Préparation de la requête de suppression
    QSqlQuery query;
    query.prepare("DELETE FROM employes WHERE id_employe = :id");
    query.bindValue(":id", id);

    // Exécution de la suppression et vérification du succès
    if (query.exec()) {
        QMessageBox::information(this, "Suppression", "Employé supprimé avec succès.");
        mettreAJourAffichage();  // Rafraîchit l'affichage après la suppression
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression de l'employé : " + query.lastError().text());
    }
}






void MainWindow::mettreAJourAffichage()
{
    // Update the display in the table view with the latest employee data
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT id_employe, nom, prenom, email, poste, adresse, etat_civil, nombre_enfants, numero_telephone, date_embauche FROM employes");
    ui->employesTreeView->setModel(model);
}
void MainWindow::rechercherEmployeParNom()
{
    // Récupère le nom entré par l'utilisateur
    QString nomRecherche = ui->nomRechercheLineEdit->text();

    // Vérifie que le champ n'est pas vide
    if (nomRecherche.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un nom pour la recherche.");
        return;
    }

    // Crée un modèle pour afficher les résultats de recherche
    QSqlQueryModel *model = new QSqlQueryModel();

    // Prépare et exécute la requête SQL pour rechercher les employés par nom
    QSqlQuery query;
    query.prepare("SELECT id_employe, nom, prenom, email, poste, adresse, etat_civil, nombre_enfants, numero_telephone, date_embauche "
                  "FROM employes WHERE nom LIKE :nom");
    query.bindValue(":nom", "%" + nomRecherche + "%");  // Utilise LIKE pour une recherche partielle
    query.exec();

    // Affecte le résultat de la requête au modèle
    model->setQuery(std::move(query));

    // Affiche les résultats dans la vue `employesTreeView`
    ui->employesTreeView->setModel(model);
}

void MainWindow::rechercherEmployeParPoste()
{
    // Récupère le poste entré par l'utilisateur
    QString posteRecherche = ui->posteRechercheLineEdit->text().trimmed();

    // Vérifie que le champ de recherche n'est pas vide
    if (posteRecherche.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un poste pour la recherche.");
        return;
    }

    // Crée une requête SQL pour rechercher les employés par poste
    QSqlQuery query;
    query.prepare("SELECT id_employe, nom, prenom, email, poste, adresse, etat_civil, nombre_enfants, numero_telephone, date_embauche "
                  "FROM employes WHERE poste LIKE :poste");
    query.bindValue(":poste", "%" + posteRecherche + "%");  // Utilise LIKE pour une recherche partielle

    // Exécute la requête et vérifie si elle réussit
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la recherche : " + query.lastError().text());
        return;
    }

    // Vérifie si des résultats ont été trouvés
    if (!query.next()) {
        QMessageBox::information(this, "Résultat", "Aucun employé trouvé pour le poste spécifié.");
        return;
    }

    // Crée un modèle pour afficher les résultats de la recherche
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(std::move(query));  // Passe la requête par déplacement

    // Affiche les résultats dans la vue `employesTreeView`
    ui->employesTreeView->setModel(model);
}


void MainWindow::trierEmployesParAnciennete() {
    // Créer un modèle pour exécuter la requête
    QSqlQueryModel *model = new QSqlQueryModel;

    // Exécuter la requête de tri
    model->setQuery("SELECT id_employe, nom, prenom, email, poste, adresse, etat_civil, nombre_enfants, numero_telephone, date_embauche "
                    "FROM employes ORDER BY TO_DATE(date_embauche, 'DD/MM/YYYY') ASC");

    // Vérifier s'il y a des erreurs dans la requête
    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL:" << model->lastError();
    } else {
        // Dissocier le modèle actuel pour forcer le rafraîchissement
        ui->employesTreeView->setModel(nullptr);

        // Appliquer le nouveau modèle à la vue pour mettre à jour l'affichage
        ui->employesTreeView->setModel(model);

        // Ajuster les colonnes à la taille de leur contenu
        for (int col = 0; col < model->columnCount(); ++col) {
            ui->employesTreeView->resizeColumnToContents(col);
        }
    }
}



// Destructor
MainWindow::~MainWindow() {
    delete ui;
}












