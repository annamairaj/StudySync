#ifndef TEST_FRIENDS_H
#define TEST_FRIENDS_H

// Forward declare test class
class StudySyncTests;

// Add these friend declarations to your class headers:

/*
In welcomescreen.h, add to the WelcomeScreen class:
    friend class StudySyncTests;

In dashboard.h, add to the DashboardWindow class:
    friend class StudySyncTests;

In addcarddialog.h, add to the AddCardDialog class:
    friend class StudySyncTests;
*/

// Alternative: Make methods public in test builds
#ifdef TESTING_BUILD
#define PRIVATE_FOR_TESTING public
#else
#define PRIVATE_FOR_TESTING private
#endif

#endif // TEST_FRIENDS_H
