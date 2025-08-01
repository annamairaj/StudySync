#include <QtTest/QtTest>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QSignalSpy>
#include <QTextEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QCryptographicHash>

// Include your application headers
#include "welcomescreen.h"
#include "addcarddialog.h"
#include "dashboard.h"
#include "databasemanager.h"

// // Define Card struct if not in headers
// struct Card {
//     int id;
//     QString question;
//     QString answer;
// };

class StudySyncTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases
    void test_emailValidation();
    void test_addCardDialog_validInput();
    void test_addCardDialog_emptyFields();
    void test_userRegistration();
    void test_userLogin_validCredentials();
    void test_userLogin_invalidCredentials();
    void test_cardDatabaseOperations();
    void test_dashboardNavigation();
    void test_passwordHashing();
    void test_databaseInitialization();

private:
    QTemporaryDir *tempDir;
    QString testDbPath;
    WelcomeScreen *welcomeScreen;
    DashboardWindow *dashboard;
    AddCardDialog *addCardDialog;
};

void StudySyncTests::initTestCase()
{
    // Setup test database in temporary directory
    tempDir = new QTemporaryDir();
    QVERIFY(tempDir->isValid());

    testDbPath = tempDir->path() + "/test_app.db";

    // Initialize test database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "testConnection");
    db.setDatabaseName(testDbPath);
    QVERIFY(db.open());

    // Create test tables
    QSqlQuery query(db);
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS cards (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            question TEXT NOT NULL,
            answer TEXT NOT NULL,
            FOREIGN KEY(user_id) REFERENCES users(id)
        )
    )");
}

void StudySyncTests::cleanupTestCase()
{
    QSqlDatabase::removeDatabase("testConnection");
    delete tempDir;
}

void StudySyncTests::init()
{
    // Create fresh instances for each test
    welcomeScreen = new WelcomeScreen();
    dashboard = new DashboardWindow();
    addCardDialog = new AddCardDialog();
}

void StudySyncTests::cleanup()
{
    delete welcomeScreen;
    delete dashboard;
    delete addCardDialog;

    // Clean test data between tests
    QSqlDatabase db = QSqlDatabase::database("testConnection");
    QSqlQuery query(db);
    query.exec("DELETE FROM cards");
    query.exec("DELETE FROM users");
}

// Test 1: Email Validation
void StudySyncTests::test_emailValidation()
{
    // Test valid emails
    QVERIFY(welcomeScreen->isValidEmail("test@example.com"));
    QVERIFY(welcomeScreen->isValidEmail("user.name+tag@domain.co.uk"));
    QVERIFY(welcomeScreen->isValidEmail("simple@test.org"));

    // Test invalid emails
    QVERIFY(!welcomeScreen->isValidEmail("invalid-email"));
    QVERIFY(!welcomeScreen->isValidEmail("@domain.com"));
    QVERIFY(!welcomeScreen->isValidEmail("user@"));
    QVERIFY(!welcomeScreen->isValidEmail("user space@example.com"));
    QVERIFY(!welcomeScreen->isValidEmail(""));
}

// Test 2: Add Card Dialog - Valid Input
void StudySyncTests::test_addCardDialog_validInput()
{
    // Set valid question and answer
    QTextEdit *questionEdit = addCardDialog->findChild<QTextEdit*>("questionTextEdit");
    QTextEdit *answerEdit = addCardDialog->findChild<QTextEdit*>("answerTextEdit");

    QVERIFY(questionEdit != nullptr);
    QVERIFY(answerEdit != nullptr);

    questionEdit->setPlainText("What is the capital of France?");
    answerEdit->setPlainText("Paris");

    QCOMPARE(addCardDialog->getQuestion(), "What is the capital of France?");
    QCOMPARE(addCardDialog->getAnswer(), "Paris");

    // Test create button with valid input
    QPushButton *createButton = addCardDialog->findChild<QPushButton*>("createCardButton");
    QVERIFY(createButton != nullptr);

    QSignalSpy acceptedSpy(addCardDialog, &QDialog::accepted);
    QTest::mouseClick(createButton, Qt::LeftButton);
    QCOMPARE(acceptedSpy.count(), 1);
}

// Test 3: Add Card Dialog - Empty Fields
void StudySyncTests::test_addCardDialog_emptyFields()
{
    // Test with empty question
    QTextEdit *questionEdit = addCardDialog->findChild<QTextEdit*>("questionTextEdit");
    QTextEdit *answerEdit = addCardDialog->findChild<QTextEdit*>("answerTextEdit");

    questionEdit->setPlainText("");
    answerEdit->setPlainText("Some answer");

    QPushButton *createButton = addCardDialog->findChild<QPushButton*>("createCardButton");
    QSignalSpy rejectedSpy(addCardDialog, &QDialog::rejected);

    // Should show warning and not accept
    QTest::mouseClick(createButton, Qt::LeftButton);
    QCOMPARE(rejectedSpy.count(), 0); // Should not be rejected, just show warning

    // Test with both fields empty
    answerEdit->setPlainText("");
    QTest::mouseClick(createButton, Qt::LeftButton);
    QCOMPARE(rejectedSpy.count(), 0);
}

// Test 4: User Registration
void StudySyncTests::test_userRegistration()
{
    // Test successful registration
    bool result = welcomeScreen->registerNewUser("testuser", "test@example.com", "password123");
    QVERIFY(result);

    // Test duplicate username
    bool duplicateResult = welcomeScreen->registerNewUser("testuser", "another@example.com", "password456");
    QVERIFY(!duplicateResult);

    // Test duplicate email
    bool duplicateEmailResult = welcomeScreen->registerNewUser("anotheruser", "test@example.com", "password789");
    QVERIFY(!duplicateEmailResult);
}

// Test 5: User Login - Valid Credentials
void StudySyncTests::test_userLogin_validCredentials()
{
    // First register a user
    welcomeScreen->registerNewUser("logintest", "login@test.com", "testpass");

    // Test login with username
    bool loginResult = welcomeScreen->validateLogin("logintest", "testpass");
    QVERIFY(loginResult);

    // Test login with email
    bool emailLoginResult = welcomeScreen->validateLogin("login@test.com", "testpass");
    QVERIFY(emailLoginResult);
}

// Test 6: User Login - Invalid Credentials
void StudySyncTests::test_userLogin_invalidCredentials()
{
    // Register a user first
    welcomeScreen->registerNewUser("validuser", "valid@test.com", "correctpass");

    // Test with wrong password
    bool wrongPassResult = welcomeScreen->validateLogin("validuser", "wrongpass");
    QVERIFY(!wrongPassResult);

    // Test with non-existent user
    bool nonExistentResult = welcomeScreen->validateLogin("nonexistent", "anypass");
    QVERIFY(!nonExistentResult);

    // Test with empty credentials
    bool emptyResult = welcomeScreen->validateLogin("", "");
    QVERIFY(!emptyResult);
}

// Test 7: Card Database Operations
void StudySyncTests::test_cardDatabaseOperations()
{
    // Test saving a card
    int cardId = dashboard->saveCardToDatabase("Test Question", "Test Answer");
    QVERIFY(cardId > 0);

    // Test loading cards
    QList<Card> cards = dashboard->loadAllCards();
    QVERIFY(cards.size() >= 1);

    bool foundCard = false;
    for (const Card &card : cards) {
        if (card.id == cardId) {
            QCOMPARE(card.question, "Test Question");
            QCOMPARE(card.answer, "Test Answer");
            foundCard = true;
            break;
        }
    }
    QVERIFY(foundCard);

    // Test deleting a card
    dashboard->deleteCardFromDatabase(cardId);
    QList<Card> cardsAfterDelete = dashboard->loadAllCards();

    bool cardStillExists = false;
    for (const Card &card : cardsAfterDelete) {
        if (card.id == cardId) {
            cardStillExists = true;
            break;
        }
    }
    QVERIFY(!cardStillExists);
}

// Test 8: Dashboard Navigation
void StudySyncTests::test_dashboardNavigation()
{
    QStackedWidget *stackedWidget = dashboard->findChild<QStackedWidget*>("homeStackedWidget");
    QVERIFY(stackedWidget != nullptr);

    // Test dashboard button
    QPushButton *dashboardBtn = dashboard->findChild<QPushButton*>("dashboardButton");
    QVERIFY(dashboardBtn != nullptr);
    QTest::mouseClick(dashboardBtn, Qt::LeftButton);
    QCOMPARE(stackedWidget->currentIndex(), 0);

    // Test subjects button
    QPushButton *subjectsBtn = dashboard->findChild<QPushButton*>("subjectsButton");
    QVERIFY(subjectsBtn != nullptr);
    QTest::mouseClick(subjectsBtn, Qt::LeftButton);
    QCOMPARE(stackedWidget->currentIndex(), 1);

    // Test certificates button
    QPushButton *certificatesBtn = dashboard->findChild<QPushButton*>("certificatesButton");
    QVERIFY(certificatesBtn != nullptr);
    QTest::mouseClick(certificatesBtn, Qt::LeftButton);
    QCOMPARE(stackedWidget->currentIndex(), 2);
}

// Test 9: Password Hashing Consistency
void StudySyncTests::test_passwordHashing()
{
    QString password = "testpassword123";

    // Register user with password
    welcomeScreen->registerNewUser("hashtest", "hash@test.com", password);

    // Verify login works (which means hash was stored and retrieved correctly)
    bool loginWorked = welcomeScreen->validateLogin("hashtest", password);
    QVERIFY(loginWorked);

    // Verify different password doesn't work
    bool wrongPasswordFailed = welcomeScreen->validateLogin("hashtest", "wrongpassword");
    QVERIFY(!wrongPasswordFailed);

    // Verify that the actual hash is stored, not plain text
    QSqlQuery query(QSqlDatabase::database("testConnection"));
    query.prepare("SELECT password_hash FROM users WHERE username = ?");
    query.addBindValue("hashtest");
    QVERIFY(query.exec());
    QVERIFY(query.next());

    QString storedHash = query.value(0).toString();
    QVERIFY(storedHash != password); // Should not be plain text
    QVERIFY(storedHash.length() == 64); // SHA-256 hex length
}

// Test 10: Database Initialization
void StudySyncTests::test_databaseInitialization()
{
    // Test that DatabaseManager initializes correctly
    bool initResult = DatabaseManager::initialize();
    QVERIFY(initResult);

    // Verify database connection exists
    QSqlDatabase db = DatabaseManager::db();
    QVERIFY(db.isOpen());

    // Test that required tables exist
    QStringList tables = db.tables();
    QVERIFY(tables.contains("users"));
    QVERIFY(tables.contains("cards"));

    // Test table structure
    QSqlQuery query(db);
    query.exec("PRAGMA table_info(users)");

    QStringList userColumns;
    while (query.next()) {
        userColumns << query.value(1).toString(); // column name
    }

    QVERIFY(userColumns.contains("id"));
    QVERIFY(userColumns.contains("username"));
    QVERIFY(userColumns.contains("email"));
    QVERIFY(userColumns.contains("password_hash"));
}

// Test runner
QTEST_MAIN(StudySyncTests)
#include "test_studysync.moc"
