#include "boat.h"

// Constructeur par défaut
Employe::Employe() {}

// Constructeur avec paramètres
Employe::Employe(int id_employe, QString nom, QString prenom, QString email ,QString poste, QString adresse, QString etat_civil, int nombre_enfants,
                 QString numero_telephone, QString date_embauche)
    : id_employe(id_employe), nom(nom), prenom(prenom), email(email), poste(poste), adresse(adresse),
      etat_civil(etat_civil), nombre_enfants(nombre_enfants), numero_telephone(numero_telephone), date_embauche(date_embauche)  {}

// Méthode pour ajouter un employé dans la base de données


// Méthode pour supprimer un employé de la base de données en utilisant l'id
bool Employe::supprimer(int id_employe) {
    QSqlQuery query;
    query.prepare("DELETE FROM employes WHERE id = :id");
    query.bindValue(":id_employe", id_employe);
    return query.exec();
}

// Méthode pour modifier un employé existant
bool Employe::modifier(int id_employe) {
    QSqlQuery query;
    query.prepare("UPDATE employes SET nom = :nom, prenom = :prenom, email = :email, poste = :poste, adresse = :adresse, "
                  "etat_civil = :etat_civil , nombre_enfants = :nombre_enfants, numero_telephone = :numero_telephone, "
                  "date_embauche = :date_embauche");
    query.bindValue(":id_employe", id_employe);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":adresse", adresse);
    query.bindValue(":etat_civil", etat_civil);
    query.bindValue(":nombre_enfants", nombre_enfants);
    query.bindValue(":numero_telephone", numero_telephone);
    query.bindValue(":date_embauche", date_embauche);

    return query.exec();
}

// Méthode pour afficher tous les employés
QSqlQueryModel* Employe::afficher() {
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT id_employe, nom, prenom, email, poste, adresse, etat_civil, nombre_enfants, numero_telephone, date_embauche");
    return model;
}
