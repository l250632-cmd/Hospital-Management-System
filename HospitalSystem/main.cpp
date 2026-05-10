// Disable MSVC unsafe function warnings
#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cstdio>
#include "HospitalSystem.h"
#include "MyString.h"

// ============================================================
//  Simple UI helpers
// ============================================================

// Draw a filled rounded rectangle (approximated with a rectangle)
void drawRoundedRect(sf::RenderWindow& win, float x, float y,
                     float w, float h, sf::Color fill, sf::Color outline = sf::Color::Transparent,
                     float outlineThick = 0.f) {
    sf::RectangleShape rect(sf::Vector2f(w, h));
    rect.setPosition(x, y);
    rect.setFillColor(fill);
    rect.setOutlineColor(outline);
    rect.setOutlineThickness(outlineThick);
    win.draw(rect);
}

// ============================================================
//  Screen/state management
// ============================================================
enum Screen {
    SCREEN_LOGIN,
    SCREEN_PATIENT_MENU,
    SCREEN_DOCTOR_MENU,
    SCREEN_ADMIN_MENU,
    SCREEN_BOOK_APPT,
    SCREEN_CANCEL_APPT,
    SCREEN_VIEW_APPTS,
    SCREEN_VIEW_BILLS,
    SCREEN_PAY_BILL,
    SCREEN_TOPUP,
    SCREEN_VIEW_RECORDS,
    SCREEN_DOCTOR_TODAY,
    SCREEN_MARK_COMPLETE,
    SCREEN_MARK_NOSHOW,
    SCREEN_WRITE_PRESC,
    SCREEN_DOCTOR_PATIENT_HISTORY,
    SCREEN_ADMIN_ADD_DOCTOR,
    SCREEN_ADMIN_VIEW_ALL,
    SCREEN_ADMIN_REPORT,
    SCREEN_ADMIN_SECURITY,
    SCREEN_ADMIN_DISCHARGE,
    SCREEN_MESSAGE  // generic message screen
};

// ============================================================
//  Input field helper
// ============================================================
struct InputField {
    char buf[300];
    int  len;
    bool active;
    bool isPassword;

    InputField() {
        buf[0] = '\0';
        len = 0;
        active = false;
        isPassword = false;
    }

    void clear() {
        buf[0] = '\0';
        len = 0;
    }

    void addChar(char c) {
        if (len < 299) {
            buf[len++] = c;
            buf[len] = '\0';
        }
    }

    void backspace() {
        if (len > 0) {
            len--;
            buf[len] = '\0';
        }
    }

    // Get display string (masked if password)
    void getDisplay(char* out) const {
        if (!isPassword) {
            myStrCopy(out, buf);
            return;
        }
        for (int i = 0; i < len; i++) out[i] = '*';
        out[len] = '\0';
    }
};

// ============================================================
//  Draw helpers
// ============================================================

void drawText(sf::RenderWindow& win, sf::Font& font, const char* text,
              float x, float y, int size, sf::Color color) {
    sf::Text t;
    t.setFont(font);
    t.setString(text);
    t.setCharacterSize(size);
    t.setFillColor(color);
    t.setPosition(x, y);
    win.draw(t);
}

bool drawButton(sf::RenderWindow& win, sf::Font& font, const char* label,
                float x, float y, float w, float h,
                sf::Color bg, sf::Color fg,
                sf::Vector2i mousePos, bool clicked) {
    sf::FloatRect r(x, y, w, h);
    bool hover = r.contains((float)mousePos.x, (float)mousePos.y);
    sf::Color actualBg = hover ? sf::Color(bg.r + 20, bg.g + 20, bg.b + 20, bg.a) : bg;
    drawRoundedRect(win, x, y, w, h, actualBg, sf::Color(200, 200, 200), 1.f);
    sf::Text t;
    t.setFont(font);
    t.setString(label);
    t.setCharacterSize(16);
    t.setFillColor(fg);
    sf::FloatRect tb = t.getLocalBounds();
    t.setPosition(x + (w - tb.width) / 2.f, y + (h - tb.height) / 2.f - 3.f);
    win.draw(t);
    return hover && clicked;
}

void drawInputField(sf::RenderWindow& win, sf::Font& font, InputField& field,
                    float x, float y, float w, float h, const char* label) {
    // label
    drawText(win, font, label, x, y - 22, 15, sf::Color(180, 180, 180));
    // box
    sf::Color border = field.active ? sf::Color(100, 180, 255) : sf::Color(80, 80, 100);
    drawRoundedRect(win, x, y, w, h, sf::Color(40, 40, 60), border, 2.f);
    // text
    char disp[300];
    field.getDisplay(disp);
    // add blinking cursor if active
    if (field.active) {
        char withCursor[302];
        myStrCopy(withCursor, disp);
        myStrCat(withCursor, "|");
        drawText(win, font, withCursor, x + 8, y + (h - 18) / 2.f, 16, sf::Color::White);
    } else {
        drawText(win, font, disp, x + 8, y + (h - 18) / 2.f, 16, sf::Color(200, 200, 200));
    }
}

// ============================================================
//  Color palette
// ============================================================
const sf::Color BG_COLOR(18, 18, 30);
const sf::Color CARD_COLOR(30, 30, 50);
const sf::Color ACCENT(70, 140, 255);
const sf::Color GREEN_BTN(40, 160, 80);
const sf::Color RED_BTN(180, 50, 50);
const sf::Color GRAY_BTN(60, 60, 80);
const sf::Color TEXT_COLOR(220, 220, 240);
const sf::Color DIM_TEXT(130, 130, 160);
const sf::Color TITLE_COLOR(100, 180, 255);

// ============================================================
//  Main
// ============================================================
int main() {
    // Window setup
    sf::RenderWindow window(sf::VideoMode(900, 650), "MediCore - Hospital Management System");
    window.setFramerateLimit(60);

    // Font
    sf::Font font;
    // Try Windows fonts first, then Linux fonts as fallback
    bool fontLoaded = font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("C:\\Windows\\Fonts\\calibri.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("C:\\Windows\\Fonts\\tahoma.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("C:\\Windows\\Fonts\\verdana.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("C:\\Windows\\Fonts\\segoeui.ttf");
    // Linux fallbacks
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
    // If still no font, show error and exit
    if (!fontLoaded) {
        // Show error using SFML itself - no windows.h needed
        sf::RenderWindow errWin(sf::VideoMode(500, 120), "Font Error");
        while (errWin.isOpen()) {
            sf::Event ev;
            while (errWin.pollEvent(ev))
                if (ev.type == sf::Event::Closed) errWin.close();
            errWin.clear(sf::Color(30, 0, 0));
            // cant draw text without font so just show a red window
            // user should copy arial.ttf next to the exe
            errWin.display();
        }
        return -1;
    }

    // Load hospital system data
    HospitalSystem hospital;
    hospital.loadAll();

    // State
    Screen currentScreen = SCREEN_LOGIN;
    int loggedInId = -1;
    int loginRole = 0; // 1=patient, 2=doctor, 3=admin
    char loggedInName[100] = "";

    // Login screen fields
    InputField loginIdField, loginPassField, loginRoleField;
    int selectedRole = 1; // 1=patient, 2=doctor, 3=admin
    int loginFailCount = 0;
    bool sessionLocked = false;

    // Generic input fields
    InputField field1, field2, field3, field4, field5;
    char field1Label[50] = "", field2Label[50] = "", field3Label[50] = "", field4Label[50] = "", field5Label[50] = "";
    int activeInputScreen = 0;

    // Message screen
    char msgTitle[100] = "";
    char msgBody[2000] = "";
    Screen msgReturnScreen = SCREEN_LOGIN;

    // Scroll offset for list screens
    int scrollOffset = 0;

    // Appointment booking context
    int bookDoctorId = -1;
    char bookDate[20] = "";
    char bookSlot[20] = "";

    // Patient appointment list (for display/cancel)
    static Appointment patientAppts[100];
    int patientApptCount = 0;

    // Bill list
    static Bill patientBills[100];
    int patientBillCount = 0;

    // Prescription list
    static Prescription patientPrescs[100];
    int patientPrescCount = 0;

    // Doctor today appointments
    static Appointment doctorAppts[100];
    int doctorApptCount = 0;

    // Misc
    char statusMsg[300] = "";
    bool showStatus = false;

    // Helper: refresh patient appointment list
    auto refreshPatientAppts = [&]() {
        patientApptCount = hospital.getPatientAppointments(loggedInId, patientAppts, 100);
    };

    auto refreshPatientBills = [&]() {
        patientBillCount = hospital.getPatientBills(loggedInId, patientBills, 100);
    };

    auto refreshDoctorAppts = [&]() {
        doctorApptCount = hospital.getDoctorTodayAppointments(loggedInId, doctorAppts, 100);
    };

    auto showMessage = [&](const char* title, const char* body, Screen ret) {
        myStrCopy(msgTitle, title);
        myStrCopy(msgBody, body);
        msgReturnScreen = ret;
        currentScreen = SCREEN_MESSAGE;
        scrollOffset = 0;
    };

    // ============================================================
    //  Main loop
    // ============================================================
    bool mouseClicked = false;
    sf::Vector2i mousePos;

    while (window.isOpen()) {
        mouseClicked = false;
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                mouseClicked = true;
                mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            }

            if (event.type == sf::Event::MouseMoved) {
                mousePos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
            }

            // Text input
            if (event.type == sf::Event::TextEntered) {
                char c = (char)event.text.unicode;
                // Feed to active input field based on screen
                auto feedActive = [&](InputField& f) {
                    if (f.active) {
                        if (c == '\b') f.backspace();
                        else if (c >= 32 && c < 127) f.addChar(c);
                    }
                };
                if (currentScreen == SCREEN_LOGIN) {
                    feedActive(loginIdField);
                    feedActive(loginPassField);
                } else {
                    feedActive(field1);
                    feedActive(field2);
                    feedActive(field3);
                    feedActive(field4);
                    feedActive(field5);
                }
            }

            // Mouse wheel scrolling
            if (event.type == sf::Event::MouseWheelScrolled) {
                scrollOffset -= (int)event.mouseWheelScroll.delta * 2;
                if (scrollOffset < 0) scrollOffset = 0;
            }
        }

        window.clear(BG_COLOR);

        // ==================================================
        //  Draw top bar
        // ==================================================
        drawRoundedRect(window, 0, 0, 900, 55, CARD_COLOR);
        drawText(window, font, "MediCore Hospital Management System", 15, 15, 22, TITLE_COLOR);
        if (loggedInId != -1) {
            char userInfo[200];
            myStrCopy(userInfo, "Logged in as: ");
            myStrCat(userInfo, loggedInName);
            drawText(window, font, userInfo, 600, 18, 14, DIM_TEXT);
        }

        // ==================================================
        //  Screen rendering
        // ==================================================

        // ---- LOGIN SCREEN ----
        if (currentScreen == SCREEN_LOGIN) {
            drawText(window, font, "Welcome to MediCore", 300, 80, 28, TITLE_COLOR);
            drawText(window, font, "Please select your role and login", 290, 115, 15, DIM_TEXT);

            // Role buttons
            float rbx = 150, rby = 160, rbw = 160, rbh = 40;
            if (drawButton(window, font, "Patient", rbx, rby, rbw, rbh,
                           selectedRole == 1 ? ACCENT : GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                selectedRole = 1;
            if (drawButton(window, font, "Doctor", rbx + 175, rby, rbw, rbh,
                           selectedRole == 2 ? ACCENT : GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                selectedRole = 2;
            if (drawButton(window, font, "Admin", rbx + 350, rby, rbw, rbh,
                           selectedRole == 3 ? ACCENT : GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                selectedRole = 3;

            // ID Field
            float fx = 250, fy = 240;
            if (mouseClicked) {
                sf::FloatRect r1(fx, fy, 400, 40);
                sf::FloatRect r2(fx, fy + 80, 400, 40);
                loginIdField.active   = r1.contains((float)mousePos.x, (float)mousePos.y);
                loginPassField.active = r2.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, loginIdField, fx, fy, 400, 40, "User ID");
            loginPassField.isPassword = true;
            drawInputField(window, font, loginPassField, fx, fy + 80, 400, 40, "Password");

            if (sessionLocked) {
                drawText(window, font, "Account locked. Contact admin.", 220, 360, 16, sf::Color(255, 100, 100));
            } else {
                // Login button
                if (drawButton(window, font, "Login", 350, 370, 200, 45,
                               GREEN_BTN, sf::Color::White, mousePos, mouseClicked)) {
                    int uid = myStrToInt(loginIdField.buf);
                    char name[100] = "";
                    bool ok = false;

                    char idStr[20];
                    myIntToStr(uid, idStr);

                    if (selectedRole == 1) {
                        ok = hospital.loginPatient(uid, loginPassField.buf, name);
                        if (ok) loginRole = 1;
                        else hospital.logSecurity("Patient", idStr, "FAILED");
                    } else if (selectedRole == 2) {
                        ok = hospital.loginDoctor(uid, loginPassField.buf, name);
                        if (ok) loginRole = 2;
                        else hospital.logSecurity("Doctor", idStr, "FAILED");
                    } else {
                        ok = hospital.loginAdmin(uid, loginPassField.buf);
                        if (ok) { loginRole = 3; myStrCopy(name, "Admin"); }
                        else hospital.logSecurity("Admin", idStr, "FAILED");
                    }

                    if (ok) {
                        loggedInId = uid;
                        myStrCopy(loggedInName, name);
                        loginFailCount = 0;
                        loginIdField.clear();
                        loginPassField.clear();
                        scrollOffset = 0;
                        if (loginRole == 1)      currentScreen = SCREEN_PATIENT_MENU;
                        else if (loginRole == 2) currentScreen = SCREEN_DOCTOR_MENU;
                        else                     currentScreen = SCREEN_ADMIN_MENU;
                    } else {
                        loginFailCount++;
                        if (loginFailCount >= 3) {
                            sessionLocked = true;
                            char lockMsg[100] = "Locked: ";
                            myStrCat(lockMsg, loginIdField.buf);
                            hospital.logSecurity(
                                selectedRole == 1 ? "Patient" : selectedRole == 2 ? "Doctor" : "Admin",
                                loginIdField.buf, "LOCKED");
                        }
                        loginPassField.clear();
                        myStrCopy(statusMsg, "Invalid credentials.");
                        showStatus = true;
                    }
                }

                if (showStatus) {
                    drawText(window, font, statusMsg, 300, 430, 15, sf::Color(255, 120, 120));
                }
            }
            drawText(window, font, "MediCore v1.0 | OOP Spring 2026", 320, 610, 13, DIM_TEXT);
        }

        // ---- PATIENT MENU ----
        else if (currentScreen == SCREEN_PATIENT_MENU) {
            char balBuf[30];
            hospital.getPatientBalance(loggedInId, balBuf);
            char header[200];
            myStrCopy(header, "Welcome, ");
            myStrCat(header, loggedInName);
            drawText(window, font, header, 30, 70, 24, TEXT_COLOR);
            char balLabel[100] = "Balance: PKR ";
            myStrCat(balLabel, balBuf);
            drawText(window, font, balLabel, 30, 100, 16, sf::Color(100, 220, 130));

            float bx = 80, by = 145, bw = 340, bh = 48, gap = 56;
            if (drawButton(window, font, "1. Book Appointment", bx, by, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                field1.clear(); field2.clear(); field3.clear(); field4.clear(); field5.clear();
                myStrCopy(field1Label, "Specialization to search");
                myStrCopy(field2Label, "Doctor ID");
                myStrCopy(field3Label, "Date (DD-MM-YYYY)");
                myStrCopy(field4Label, "Time Slot (e.g. 09:00)");
                field1.active = true;
                currentScreen = SCREEN_BOOK_APPT;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "2. Cancel Appointment", bx, by + gap, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                field1.clear();
                myStrCopy(field1Label, "Appointment ID to cancel");
                field1.active = true;
                refreshPatientAppts();
                currentScreen = SCREEN_CANCEL_APPT;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "3. View My Appointments", bx, by + gap * 2, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                refreshPatientAppts();
                currentScreen = SCREEN_VIEW_APPTS;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "4. View My Medical Records", bx, by + gap * 3, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                // load prescriptions for this patient
                patientPrescCount = 0;
                Storage<Prescription>& prescs = hospital.getPrescriptions();
                for (int i = 0; i < prescs.size(); i++) {
                    if (prescs.getAt(i)->getPatientId() == loggedInId)
                        patientPrescs[patientPrescCount++] = *prescs.getAt(i);
                }
                currentScreen = SCREEN_VIEW_RECORDS;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "5. View My Bills", bx, by + gap * 4, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                refreshPatientBills();
                currentScreen = SCREEN_VIEW_BILLS;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "6. Pay Bill", bx, by + gap * 5, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                refreshPatientBills();
                field1.clear();
                myStrCopy(field1Label, "Bill ID to pay");
                field1.active = true;
                currentScreen = SCREEN_PAY_BILL;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "7. Top Up Balance", bx, by + gap * 6, bw, bh, GREEN_BTN, sf::Color::White, mousePos, mouseClicked)) {
                field1.clear();
                myStrCopy(field1Label, "Amount to add (PKR)");
                field1.active = true;
                currentScreen = SCREEN_TOPUP;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "8. Logout", bx, by + gap * 7, bw, bh, RED_BTN, sf::Color::White, mousePos, mouseClicked)) {
                loggedInId = -1; loggedInName[0] = '\0';
                currentScreen = SCREEN_LOGIN;
                showStatus = false;
            }
        }

        // ---- BOOK APPOINTMENT ----
        else if (currentScreen == SCREEN_BOOK_APPT) {
            drawText(window, font, "Book Appointment", 30, 70, 24, TITLE_COLOR);

            float fx = 80, fy = 120, fw = 400, fh = 40;

            if (mouseClicked) {
                sf::FloatRect r1(fx, fy, fw, fh);
                sf::FloatRect r2(fx, fy + 80, fw, fh);
                sf::FloatRect r3(fx, fy + 160, fw, fh);
                sf::FloatRect r4(fx, fy + 240, fw, fh);
                field1.active = r1.contains((float)mousePos.x, (float)mousePos.y);
                field2.active = r2.contains((float)mousePos.x, (float)mousePos.y);
                field3.active = r3.contains((float)mousePos.x, (float)mousePos.y);
                field4.active = r4.contains((float)mousePos.x, (float)mousePos.y);
            }

            drawInputField(window, font, field1, fx, fy, fw, fh, field1Label);
            drawInputField(window, font, field2, fx, fy + 80, fw, fh, field2Label);
            drawInputField(window, font, field3, fx, fy + 160, fw, fh, field3Label);
            drawInputField(window, font, field4, fx, fy + 240, fw, fh, field4Label);

            // Show available slots panel
            if (myStrLen(field2.buf) > 0 && myStrLen(field3.buf) >= 10) {
                int did = myStrToInt(field2.buf);
                char slots[200];
                hospital.getAvailableSlots(did, field3.buf, slots);
                drawText(window, font, "Available slots:", 520, 120, 15, DIM_TEXT);
                drawText(window, font, slots, 520, 145, 14, sf::Color(100, 220, 130));

                // Show doctor fee
                char feeBuf[50];
                hospital.getDoctorFee(did, feeBuf);
                char feeLabel[100] = "Fee: PKR ";
                myStrCat(feeLabel, feeBuf);
                drawText(window, font, feeLabel, 520, 175, 14, sf::Color(220, 180, 80));
            }

            if (showStatus)
                drawText(window, font, statusMsg, fx, fy + 320, 15, sf::Color(255, 120, 120));

            if (drawButton(window, font, "Book", fx, fy + 360, 180, 44, GREEN_BTN, sf::Color::White, mousePos, mouseClicked)) {
                char msg[300];
                hospital.bookAppointment(loggedInId,
                    myStrToInt(field2.buf),
                    field3.buf, field4.buf, msg);
                showMessage("Booking Result", msg, SCREEN_PATIENT_MENU);
            }
            if (drawButton(window, font, "Back", fx + 200, fy + 360, 180, 44, GRAY_BTN, sf::Color::White, mousePos, mouseClicked)) {
                currentScreen = SCREEN_PATIENT_MENU; showStatus = false;
            }
        }

        // ---- CANCEL APPOINTMENT ----
        else if (currentScreen == SCREEN_CANCEL_APPT) {
            drawText(window, font, "Cancel Appointment", 30, 70, 24, TITLE_COLOR);
            drawText(window, font, "Pending Appointments:", 30, 110, 16, DIM_TEXT);

            float ly = 135;
            bool hasPending = false;
            for (int i = 0; i < patientApptCount; i++) {
                if (!myStrEqual(patientAppts[i].getStatus(), "pending")) continue;
                hasPending = true;
                char docName[100];
                hospital.getDoctorName(patientAppts[i].getDoctorId(), docName);
                char line[300];
                myStrCopy(line, "ID: ");
                char idBuf[10]; myIntToStr(patientAppts[i].getAppointmentId(), idBuf);
                myStrCat(line, idBuf);
                myStrCat(line, "  Dr. "); myStrCat(line, docName);
                myStrCat(line, "  "); myStrCat(line, patientAppts[i].getDate());
                myStrCat(line, " "); myStrCat(line, patientAppts[i].getTimeSlot());
                drawText(window, font, line, 30, ly + i * 24 - scrollOffset, 14, TEXT_COLOR);
            }
            if (!hasPending) drawText(window, font, "No pending appointments.", 30, 135, 15, DIM_TEXT);

            float fx = 80, fy = 380;
            if (mouseClicked) {
                sf::FloatRect r1(fx, fy, 400, 40);
                field1.active = r1.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, field1, fx, fy, 400, 40, field1Label);
            if (showStatus) drawText(window, font, statusMsg, fx, fy + 55, 15, sf::Color(255, 120, 120));

            if (drawButton(window, font, "Cancel Appointment", fx, fy + 80, 220, 44, RED_BTN, sf::Color::White, mousePos, mouseClicked)) {
                char msg[300];
                hospital.cancelAppointment(loggedInId, myStrToInt(field1.buf), msg);
                showMessage("Result", msg, SCREEN_PATIENT_MENU);
            }
            if (drawButton(window, font, "Back", fx + 240, fy + 80, 150, 44, GRAY_BTN, sf::Color::White, mousePos, mouseClicked)) {
                currentScreen = SCREEN_PATIENT_MENU; showStatus = false;
            }
        }

        // ---- VIEW APPOINTMENTS ----
        else if (currentScreen == SCREEN_VIEW_APPTS) {
            drawText(window, font, "My Appointments", 30, 70, 24, TITLE_COLOR);
            float ly = 110;
            // header row
            drawRoundedRect(window, 20, ly, 860, 28, sf::Color(40, 40, 65));
            drawText(window, font, "ID", 28, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Doctor", 80, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Specialization", 250, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Date", 430, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Time", 560, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Status", 650, ly + 5, 13, DIM_TEXT);
            ly += 35;

            if (patientApptCount == 0)
                drawText(window, font, "No appointments found.", 30, ly, 15, DIM_TEXT);

            // Simple bubble sort by date ascending (copy to temp array)
            Appointment* sortArr[100];
            for (int i = 0; i < patientApptCount; i++) sortArr[i] = &patientAppts[i];
            // sort
            for (int i = 0; i < patientApptCount - 1; i++)
                for (int j = 0; j < patientApptCount - i - 1; j++) {
                    const char* d1 = sortArr[j]->getDate();
                    const char* d2 = sortArr[j+1]->getDate();
                    int cmp = 0;
                    for (int k = 6; k < 10 && cmp == 0; k++) cmp = d1[k] - d2[k];
                    for (int k = 3; k < 5 && cmp == 0; k++) cmp = d1[k] - d2[k];
                    for (int k = 0; k < 2 && cmp == 0; k++) cmp = d1[k] - d2[k];
                    if (cmp > 0) { Appointment* tmp = sortArr[j]; sortArr[j] = sortArr[j+1]; sortArr[j+1] = tmp; }
                }

            for (int i = 0; i < patientApptCount; i++) {
                float row = ly + i * 26 - scrollOffset;
                if (row < 100 || row > 590) continue;
                sf::Color rowBg = (i % 2 == 0) ? sf::Color(32, 32, 50) : sf::Color(28, 28, 45);
                drawRoundedRect(window, 20, row, 860, 24, rowBg);
                char idBuf[10]; myIntToStr(sortArr[i]->getAppointmentId(), idBuf);
                char docName[100]; hospital.getDoctorName(sortArr[i]->getDoctorId(), docName);
                char spec[100]; hospital.getDoctorSpec(sortArr[i]->getDoctorId(), spec);
                drawText(window, font, idBuf, 28, row + 4, 13, TEXT_COLOR);
                drawText(window, font, docName, 80, row + 4, 13, TEXT_COLOR);
                drawText(window, font, spec, 250, row + 4, 13, TEXT_COLOR);
                drawText(window, font, sortArr[i]->getDate(), 430, row + 4, 13, TEXT_COLOR);
                drawText(window, font, sortArr[i]->getTimeSlot(), 560, row + 4, 13, TEXT_COLOR);
                sf::Color stColor = myStrEqual(sortArr[i]->getStatus(), "completed") ? sf::Color(80, 200, 80) :
                                    myStrEqual(sortArr[i]->getStatus(), "cancelled")  ? sf::Color(200, 80, 80) :
                                    myStrEqual(sortArr[i]->getStatus(), "noshow")     ? sf::Color(200, 150, 50) :
                                    sf::Color(100, 180, 255);
                drawText(window, font, sortArr[i]->getStatus(), 650, row + 4, 13, stColor);
            }

            if (drawButton(window, font, "Back", 380, 600, 140, 38, GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                currentScreen = SCREEN_PATIENT_MENU;
        }

        // ---- VIEW BILLS ----
        else if (currentScreen == SCREEN_VIEW_BILLS) {
            drawText(window, font, "My Bills", 30, 70, 24, TITLE_COLOR);
            float ly = 110;
            drawRoundedRect(window, 20, ly, 860, 28, sf::Color(40, 40, 65));
            drawText(window, font, "Bill ID", 28, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Appt ID", 120, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Amount (PKR)", 220, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Status", 380, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Date", 500, ly + 5, 13, DIM_TEXT);
            ly += 35;

            float totalUnpaid = 0.0f;
            if (patientBillCount == 0)
                drawText(window, font, "No bills found.", 30, ly, 15, DIM_TEXT);
            for (int i = 0; i < patientBillCount; i++) {
                float row = ly + i * 26 - scrollOffset;
                if (row < 100 || row > 560) continue;
                sf::Color rowBg = (i % 2 == 0) ? sf::Color(32, 32, 50) : sf::Color(28, 28, 45);
                drawRoundedRect(window, 20, row, 860, 24, rowBg);
                char bIdBuf[10]; myIntToStr(patientBills[i].getBillId(), bIdBuf);
                char aIdBuf[10]; myIntToStr(patientBills[i].getAppointmentId(), aIdBuf);
                char amtBuf[20]; myFloatToStr(patientBills[i].getAmount(), amtBuf);
                drawText(window, font, bIdBuf, 28, row + 4, 13, TEXT_COLOR);
                drawText(window, font, aIdBuf, 120, row + 4, 13, TEXT_COLOR);
                drawText(window, font, amtBuf, 220, row + 4, 13, TEXT_COLOR);
                sf::Color stC = myStrEqual(patientBills[i].getStatus(), "paid") ? sf::Color(80, 200, 80) :
                                myStrEqual(patientBills[i].getStatus(), "cancelled") ? sf::Color(180, 80, 80) :
                                sf::Color(255, 200, 60);
                drawText(window, font, patientBills[i].getStatus(), 380, row + 4, 13, stC);
                drawText(window, font, patientBills[i].getDate(), 500, row + 4, 13, TEXT_COLOR);
                if (myStrEqual(patientBills[i].getStatus(), "unpaid"))
                    totalUnpaid += patientBills[i].getAmount();
            }

            char totalLabel[100] = "Total Outstanding: PKR ";
            char totBuf[20]; myFloatToStr(totalUnpaid, totBuf);
            myStrCat(totalLabel, totBuf);
            drawText(window, font, totalLabel, 30, 570, 15, sf::Color(255, 200, 60));

            if (drawButton(window, font, "Back", 380, 600, 140, 38, GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                currentScreen = SCREEN_PATIENT_MENU;
        }

        // ---- PAY BILL ----
        else if (currentScreen == SCREEN_PAY_BILL) {
            drawText(window, font, "Pay Bill", 30, 70, 24, TITLE_COLOR);
            drawText(window, font, "Unpaid Bills:", 30, 110, 16, DIM_TEXT);
            float ly = 135;
            bool hasUnpaid = false;
            for (int i = 0; i < patientBillCount; i++) {
                if (!myStrEqual(patientBills[i].getStatus(), "unpaid")) continue;
                hasUnpaid = true;
                char line[200];
                char bIdBuf[10]; myIntToStr(patientBills[i].getBillId(), bIdBuf);
                char amtBuf[20]; myFloatToStr(patientBills[i].getAmount(), amtBuf);
                myStrCopy(line, "Bill ID: "); myStrCat(line, bIdBuf);
                myStrCat(line, "  |  Amount: PKR "); myStrCat(line, amtBuf);
                myStrCat(line, "  |  Date: "); myStrCat(line, patientBills[i].getDate());
                drawText(window, font, line, 30, ly, 14, TEXT_COLOR);
                ly += 26;
            }
            if (!hasUnpaid) drawText(window, font, "No unpaid bills.", 30, 135, 15, DIM_TEXT);

            float fx = 80, fy = 380;
            if (mouseClicked) {
                sf::FloatRect r(fx, fy, 400, 40);
                field1.active = r.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, field1, fx, fy, 400, 40, field1Label);
            if (showStatus) drawText(window, font, statusMsg, fx, fy + 55, 15, sf::Color(255, 120, 120));

            if (drawButton(window, font, "Pay", fx, fy + 80, 180, 44, GREEN_BTN, sf::Color::White, mousePos, mouseClicked)) {
                char msg[300];
                hospital.payBill(loggedInId, myStrToInt(field1.buf), msg);
                showMessage("Payment Result", msg, SCREEN_PATIENT_MENU);
            }
            if (drawButton(window, font, "Back", fx + 200, fy + 80, 150, 44, GRAY_BTN, sf::Color::White, mousePos, mouseClicked)) {
                currentScreen = SCREEN_PATIENT_MENU; showStatus = false;
            }
        }

        // ---- TOP UP ----
        else if (currentScreen == SCREEN_TOPUP) {
            drawText(window, font, "Top Up Balance", 30, 70, 24, TITLE_COLOR);
            float fx = 200, fy = 200;
            if (mouseClicked) {
                sf::FloatRect r(fx, fy, 500, 44);
                field1.active = r.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, field1, fx, fy, 500, 44, field1Label);
            if (showStatus) drawText(window, font, statusMsg, fx, fy + 60, 15, sf::Color(255, 120, 120));

            if (drawButton(window, font, "Add Balance", fx, fy + 90, 220, 44, GREEN_BTN, sf::Color::White, mousePos, mouseClicked)) {
                float amt = myStrToFloat(field1.buf);
                char msg[300];
                hospital.topUpBalance(loggedInId, amt, msg);
                showMessage("Top Up Result", msg, SCREEN_PATIENT_MENU);
            }
            if (drawButton(window, font, "Back", fx + 240, fy + 90, 150, 44, GRAY_BTN, sf::Color::White, mousePos, mouseClicked)) {
                currentScreen = SCREEN_PATIENT_MENU; showStatus = false;
            }
        }

        // ---- VIEW MEDICAL RECORDS ----
        else if (currentScreen == SCREEN_VIEW_RECORDS) {
            drawText(window, font, "My Medical Records", 30, 70, 24, TITLE_COLOR);
            float ly = 110;
            if (patientPrescCount == 0)
                drawText(window, font, "No medical records found.", 30, ly, 15, DIM_TEXT);

            for (int i = 0; i < patientPrescCount; i++) {
                float row = ly + i * 80 - scrollOffset;
                if (row > 580 || row < 100) continue;
                drawRoundedRect(window, 20, row, 860, 72, sf::Color(32, 32, 52));
                char docName[100]; hospital.getDoctorName(patientPrescs[i].getDoctorId(), docName);
                char line1[200];
                myStrCopy(line1, patientPrescs[i].getDate());
                myStrCat(line1, "  |  Dr. "); myStrCat(line1, docName);
                drawText(window, font, line1, 30, row + 6, 14, TITLE_COLOR);
                char medLine[600] = "Medicines: ";
                myStrCat(medLine, patientPrescs[i].getMedicines());
                drawText(window, font, medLine, 30, row + 26, 13, TEXT_COLOR);
                char noteLine[400] = "Notes: ";
                myStrCat(noteLine, patientPrescs[i].getNotes());
                drawText(window, font, noteLine, 30, row + 48, 13, DIM_TEXT);
            }

            if (drawButton(window, font, "Back", 380, 600, 140, 38, GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                currentScreen = SCREEN_PATIENT_MENU;
        }

        // ---- DOCTOR MENU ----
        else if (currentScreen == SCREEN_DOCTOR_MENU) {
            char header[200] = "Welcome, Dr. ";
            myStrCat(header, loggedInName);
            drawText(window, font, header, 30, 70, 24, TEXT_COLOR);

            char spec[100];
            hospital.getDoctorSpec(loggedInId, spec);
            char specLabel[200] = "Specialization: ";
            myStrCat(specLabel, spec);
            drawText(window, font, specLabel, 30, 100, 15, DIM_TEXT);

            float bx = 80, by = 145, bw = 340, bh = 48, gap = 56;
            if (drawButton(window, font, "1. View Today's Appointments", bx, by, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                refreshDoctorAppts();
                currentScreen = SCREEN_DOCTOR_TODAY;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "2. Mark Appointment Complete", bx, by + gap, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                refreshDoctorAppts();
                field1.clear();
                myStrCopy(field1Label, "Appointment ID");
                field1.active = true;
                currentScreen = SCREEN_MARK_COMPLETE;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "3. Mark Appointment No-Show", bx, by + gap * 2, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                refreshDoctorAppts();
                field1.clear();
                myStrCopy(field1Label, "Appointment ID");
                field1.active = true;
                currentScreen = SCREEN_MARK_NOSHOW;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "4. Write Prescription", bx, by + gap * 3, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                field1.clear(); field2.clear(); field3.clear();
                myStrCopy(field1Label, "Appointment ID");
                myStrCopy(field2Label, "Medicines (e.g. Med1 500mg;Med2 250mg)");
                myStrCopy(field3Label, "Notes (max 300 chars)");
                field1.active = true;
                currentScreen = SCREEN_WRITE_PRESC;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "5. View Patient History", bx, by + gap * 4, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                field1.clear();
                myStrCopy(field1Label, "Enter Patient ID");
                field1.active = true;
                currentScreen = SCREEN_DOCTOR_PATIENT_HISTORY;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "6. Logout", bx, by + gap * 5, bw, bh, RED_BTN, sf::Color::White, mousePos, mouseClicked)) {
                loggedInId = -1; loggedInName[0] = '\0';
                currentScreen = SCREEN_LOGIN;
            }
        }

        // ---- DOCTOR TODAY APPOINTMENTS ----
        else if (currentScreen == SCREEN_DOCTOR_TODAY) {
            char today[20]; hospital.getTodayStr(today);
            char header[100] = "Today's Appointments - ";
            myStrCat(header, today);
            drawText(window, font, header, 30, 70, 22, TITLE_COLOR);

            float ly = 110;
            drawRoundedRect(window, 20, ly, 860, 28, sf::Color(40, 40, 65));
            drawText(window, font, "Appt ID", 28, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Patient", 130, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Time Slot", 380, ly + 5, 13, DIM_TEXT);
            drawText(window, font, "Status", 500, ly + 5, 13, DIM_TEXT);
            ly += 35;

            if (doctorApptCount == 0)
                drawText(window, font, "No appointments scheduled for today.", 30, ly, 15, DIM_TEXT);

            for (int i = 0; i < doctorApptCount; i++) {
                float row = ly + i * 26 - scrollOffset;
                if (row < 100 || row > 580) continue;
                sf::Color rowBg = (i % 2 == 0) ? sf::Color(32, 32, 50) : sf::Color(28, 28, 45);
                drawRoundedRect(window, 20, row, 860, 24, rowBg);
                char aIdBuf[10]; myIntToStr(doctorAppts[i].getAppointmentId(), aIdBuf);
                char patName[100]; hospital.getPatientName(doctorAppts[i].getPatientId(), patName);
                drawText(window, font, aIdBuf, 28, row + 4, 13, TEXT_COLOR);
                drawText(window, font, patName, 130, row + 4, 13, TEXT_COLOR);
                drawText(window, font, doctorAppts[i].getTimeSlot(), 380, row + 4, 13, TEXT_COLOR);
                sf::Color stC = myStrEqual(doctorAppts[i].getStatus(), "completed") ? sf::Color(80, 200, 80) :
                                sf::Color(100, 180, 255);
                drawText(window, font, doctorAppts[i].getStatus(), 500, row + 4, 13, stC);
            }

            if (drawButton(window, font, "Back", 380, 600, 140, 38, GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                currentScreen = SCREEN_DOCTOR_MENU;
        }

        // ---- MARK COMPLETE / NO-SHOW ----
        else if (currentScreen == SCREEN_MARK_COMPLETE || currentScreen == SCREEN_MARK_NOSHOW) {
            bool isComplete = (currentScreen == SCREEN_MARK_COMPLETE);
            drawText(window, font, isComplete ? "Mark Appointment Complete" : "Mark Appointment No-Show",
                     30, 70, 24, TITLE_COLOR);
            // Show today's appointments
            drawText(window, font, "Today's pending appointments:", 30, 110, 15, DIM_TEXT);
            float ly = 135;
            for (int i = 0; i < doctorApptCount; i++) {
                if (!myStrEqual(doctorAppts[i].getStatus(), "pending")) continue;
                char line[200];
                char aIdBuf[10]; myIntToStr(doctorAppts[i].getAppointmentId(), aIdBuf);
                char patName[100]; hospital.getPatientName(doctorAppts[i].getPatientId(), patName);
                myStrCopy(line, "ID: "); myStrCat(line, aIdBuf);
                myStrCat(line, "  Patient: "); myStrCat(line, patName);
                myStrCat(line, "  Time: "); myStrCat(line, doctorAppts[i].getTimeSlot());
                drawText(window, font, line, 30, ly + i * 24, 14, TEXT_COLOR);
            }
            float fx = 80, fy = 380;
            if (mouseClicked) {
                sf::FloatRect r(fx, fy, 400, 40);
                field1.active = r.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, field1, fx, fy, 400, 40, field1Label);

            sf::Color actionColor = isComplete ? GREEN_BTN : RED_BTN;
            const char* actionLabel = isComplete ? "Mark Complete" : "Mark No-Show";
            if (drawButton(window, font, actionLabel, fx, fy + 70, 220, 44, actionColor, sf::Color::White, mousePos, mouseClicked)) {
                char msg[300];
                if (isComplete)
                    hospital.markAppointmentComplete(loggedInId, myStrToInt(field1.buf), msg);
                else
                    hospital.markAppointmentNoShow(loggedInId, myStrToInt(field1.buf), msg);
                showMessage("Result", msg, SCREEN_DOCTOR_MENU);
            }
            if (drawButton(window, font, "Back", fx + 240, fy + 70, 150, 44, GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                currentScreen = SCREEN_DOCTOR_MENU;
        }

        // ---- WRITE PRESCRIPTION ----
        else if (currentScreen == SCREEN_WRITE_PRESC) {
            drawText(window, font, "Write Prescription", 30, 70, 24, TITLE_COLOR);
            float fx = 80, fy = 120, fw = 730, fh = 40;
            if (mouseClicked) {
                sf::FloatRect r1(fx, fy, fw, fh);
                sf::FloatRect r2(fx, fy + 80, fw, fh);
                sf::FloatRect r3(fx, fy + 160, fw, fh);
                field1.active = r1.contains((float)mousePos.x, (float)mousePos.y);
                field2.active = r2.contains((float)mousePos.x, (float)mousePos.y);
                field3.active = r3.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, field1, fx, fy, fw, fh, field1Label);
            drawInputField(window, font, field2, fx, fy + 80, fw, fh, field2Label);
            drawInputField(window, font, field3, fx, fy + 160, fw, fh, field3Label);

            if (drawButton(window, font, "Save Prescription", fx, fy + 240, 220, 44, GREEN_BTN, sf::Color::White, mousePos, mouseClicked)) {
                char msg[300];
                hospital.writePrescription(loggedInId, myStrToInt(field1.buf),
                                            field2.buf, field3.buf, msg);
                showMessage("Result", msg, SCREEN_DOCTOR_MENU);
            }
            if (drawButton(window, font, "Back", fx + 240, fy + 240, 150, 44, GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                currentScreen = SCREEN_DOCTOR_MENU;
        }

        // ---- DOCTOR VIEW PATIENT HISTORY ----
        else if (currentScreen == SCREEN_DOCTOR_PATIENT_HISTORY) {
            drawText(window, font, "View Patient Medical History", 30, 70, 24, TITLE_COLOR);

            float fx = 80, fy = 110, fw = 400, fh = 40;
            if (mouseClicked) {
                sf::FloatRect r(fx, fy, fw, fh);
                field1.active = r.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, field1, fx, fy, fw, fh, field1Label);

            if (drawButton(window, font, "Load History", fx + 420, fy, 160, fh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                int targetPatientId = myStrToInt(field1.buf);
                // Validate: patient exists AND has at least one completed appointment with this doctor
                bool hasAccess = false;
                Storage<Appointment>& appts = hospital.getAppointments();
                for (int i = 0; i < appts.size(); i++) {
                    Appointment* a = appts.getAt(i);
                    if (a->getPatientId() == targetPatientId &&
                        a->getDoctorId() == loggedInId &&
                        myStrEqual(a->getStatus(), "completed")) {
                        hasAccess = true;
                        break;
                    }
                }
                if (!hasAccess) {
                    showMessage("Access Denied",
                        "Access denied. You can only view records of your own patients.",
                        SCREEN_DOCTOR_PATIENT_HISTORY);
                } else {
                    // Load prescriptions written by this doctor for this patient
                    patientPrescCount = 0;
                    Storage<Prescription>& prescs = hospital.getPrescriptions();
                    for (int i = 0; i < prescs.size(); i++) {
                        Prescription* pr = prescs.getAt(i);
                        if (pr->getPatientId() == targetPatientId &&
                            pr->getDoctorId() == loggedInId) {
                            patientPrescs[patientPrescCount++] = *pr;
                        }
                    }
                    // Sort descending by date (simple bubble sort)
                    for (int i = 0; i < patientPrescCount - 1; i++) {
                        for (int j = 0; j < patientPrescCount - i - 1; j++) {
                            const char* d1 = patientPrescs[j].getDate();
                            const char* d2 = patientPrescs[j+1].getDate();
                            int cmp = 0;
                            for (int k = 6; k < 10 && cmp == 0; k++) cmp = d1[k] - d2[k];
                            for (int k = 3; k < 5 && cmp == 0; k++) cmp = d1[k] - d2[k];
                            for (int k = 0; k < 2 && cmp == 0; k++) cmp = d1[k] - d2[k];
                            if (cmp < 0) { // descending: swap if earlier
                                Prescription tmp = patientPrescs[j];
                                patientPrescs[j] = patientPrescs[j+1];
                                patientPrescs[j+1] = tmp;
                            }
                        }
                    }
                    // Show results
                    char patName[100];
                    hospital.getPatientName(targetPatientId, patName);
                    char histTitle[200] = "History for: ";
                    myStrCat(histTitle, patName);
                    char histBody[3000];
                    histBody[0] = '\0';
                    if (patientPrescCount == 0) {
                        myStrCopy(histBody, "No prescriptions found.");
                    } else {
                        for (int i = 0; i < patientPrescCount; i++) {
                            myStrCat(histBody, "Date: ");
                            myStrCat(histBody, patientPrescs[i].getDate());
                            myStrCat(histBody, "\nMedicines: ");
                            myStrCat(histBody, patientPrescs[i].getMedicines());
                            myStrCat(histBody, "\nNotes: ");
                            myStrCat(histBody, patientPrescs[i].getNotes());
                            myStrCat(histBody, "\n---\n");
                        }
                    }
                    showMessage(histTitle, histBody, SCREEN_DOCTOR_MENU);
                }
            }

            // Show results inline if already loaded
            float ly = 175;
            if (patientPrescCount > 0) {
                char patName[100];
                hospital.getPatientName(myStrToInt(field1.buf), patName);
                char subheader[200] = "Records for: "; myStrCat(subheader, patName);
                drawText(window, font, subheader, 30, ly, 16, sf::Color(100, 220, 130));
                ly += 30;
                for (int i = 0; i < patientPrescCount; i++) {
                    float row = ly + i * 80 - scrollOffset;
                    if (row > 560 || row < 150) continue;
                    drawRoundedRect(window, 20, row, 860, 72, sf::Color(32, 32, 52));
                    drawText(window, font, patientPrescs[i].getDate(), 28, row + 6, 14, TITLE_COLOR);
                    char medLine[600] = "Medicines: "; myStrCat(medLine, patientPrescs[i].getMedicines());
                    drawText(window, font, medLine, 28, row + 26, 13, TEXT_COLOR);
                    char ntLine[400] = "Notes: "; myStrCat(ntLine, patientPrescs[i].getNotes());
                    drawText(window, font, ntLine, 28, row + 48, 13, DIM_TEXT);
                }
            }

            if (drawButton(window, font, "Back", 380, 600, 140, 38, GRAY_BTN, sf::Color::White, mousePos, mouseClicked)) {
                currentScreen = SCREEN_DOCTOR_MENU;
                patientPrescCount = 0;
            }
        }

        // ---- ADMIN MENU ----
        else if (currentScreen == SCREEN_ADMIN_MENU) {
            drawText(window, font, "Admin Panel - MediCore", 30, 70, 24, TITLE_COLOR);

            float bx = 80, by = 110, bw = 340, bh = 44, gap = 52;
            if (drawButton(window, font, "1. Add Doctor", bx, by, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                field1.clear(); field2.clear(); field3.clear(); field4.clear(); field5.clear();
                myStrCopy(field1Label, "Doctor Name");
                myStrCopy(field2Label, "Specialization");
                myStrCopy(field3Label, "Contact (11 digits)");
                myStrCopy(field4Label, "Password (min 6 chars)");
                myStrCopy(field5Label, "Consultation Fee (PKR)");
                field4.isPassword = true;
                field1.active = true;
                currentScreen = SCREEN_ADMIN_ADD_DOCTOR;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "2. Remove Doctor", bx, by + gap, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                field1.clear();
                myStrCopy(field1Label, "Doctor ID to remove");
                field1.active = true;
                currentScreen = SCREEN_ADMIN_VIEW_ALL;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "3-5. View Records", bx, by + gap * 2, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                // Build a big report string
                char bigMsg[4000];
                bigMsg[0] = '\0';
                myStrCat(bigMsg, "=== All Patients ===\n");
                Storage<Patient>& pats = hospital.getPatients();
                for (int i = 0; i < pats.size(); i++) {
                    Patient* p = pats.getAt(i);
                    char idBuf[10]; myIntToStr(p->getId(), idBuf);
                    char balBuf[20]; myFloatToStr(p->getBalance(), balBuf);
                    char ageBuf[10]; myIntToStr(p->getAge(), ageBuf);
                    myStrCat(bigMsg, "ID:"); myStrCat(bigMsg, idBuf);
                    myStrCat(bigMsg, " Name:"); myStrCat(bigMsg, p->getName());
                    myStrCat(bigMsg, " Age:"); myStrCat(bigMsg, ageBuf);
                    myStrCat(bigMsg, " Gender:"); myStrCat(bigMsg, p->getGender());
                    myStrCat(bigMsg, " Bal:PKR "); myStrCat(bigMsg, balBuf);
                    myStrCat(bigMsg, "\n");
                }
                myStrCat(bigMsg, "\n=== All Doctors ===\n");
                Storage<Doctor>& docs = hospital.getDoctors();
                for (int i = 0; i < docs.size(); i++) {
                    Doctor* d = docs.getAt(i);
                    char idBuf[10]; myIntToStr(d->getId(), idBuf);
                    char feeBuf[20]; myFloatToStr(d->getFee(), feeBuf);
                    myStrCat(bigMsg, "ID:"); myStrCat(bigMsg, idBuf);
                    myStrCat(bigMsg, " Name:"); myStrCat(bigMsg, d->getName());
                    myStrCat(bigMsg, " Spec:"); myStrCat(bigMsg, d->getSpecialization());
                    myStrCat(bigMsg, " Fee:PKR "); myStrCat(bigMsg, feeBuf);
                    myStrCat(bigMsg, "\n");
                }
                showMessage("All Records", bigMsg, SCREEN_ADMIN_MENU);
            }
            if (drawButton(window, font, "6. View Unpaid Bills", bx, by + gap * 3, bw, bh, ACCENT, sf::Color::White, mousePos, mouseClicked)) {
                char bigMsg[3000];
                bigMsg[0] = '\0';
                myStrCat(bigMsg, "=== Unpaid Bills ===\n");
                Storage<Bill>& billsRef = hospital.getBills();
                for (int i = 0; i < billsRef.size(); i++) {
                    Bill* b = billsRef.getAt(i);
                    if (!myStrEqual(b->getStatus(), "unpaid")) continue;
                    char patName[100]; hospital.getPatientName(b->getPatientId(), patName);
                    char bIdBuf[10]; myIntToStr(b->getBillId(), bIdBuf);
                    char amtBuf[20]; myFloatToStr(b->getAmount(), amtBuf);
                    double diff = 0;
                    // check overdue
                    // (simplified - just note the date)
                    myStrCat(bigMsg, "Bill#"); myStrCat(bigMsg, bIdBuf);
                    myStrCat(bigMsg, " Patient:"); myStrCat(bigMsg, patName);
                    myStrCat(bigMsg, " PKR:"); myStrCat(bigMsg, amtBuf);
                    myStrCat(bigMsg, " Date:"); myStrCat(bigMsg, b->getDate());
                    myStrCat(bigMsg, "\n");
                }
                showMessage("Unpaid Bills", bigMsg, SCREEN_ADMIN_MENU);
            }
            if (drawButton(window, font, "7. Discharge Patient", bx, by + gap * 4, bw, bh, RED_BTN, sf::Color::White, mousePos, mouseClicked)) {
                field1.clear();
                myStrCopy(field1Label, "Patient ID to discharge");
                field1.active = true;
                currentScreen = SCREEN_ADMIN_DISCHARGE;
                scrollOffset = 0;
            }
            if (drawButton(window, font, "8. View Security Log", bx, by + gap * 5, bw, bh, GRAY_BTN, sf::Color::White, mousePos, mouseClicked)) {
                char logBuf[3000];
                hospital.getSecurityLog(logBuf, 3000);
                showMessage("Security Log", logBuf, SCREEN_ADMIN_MENU);
            }
            if (drawButton(window, font, "9. Daily Report", bx, by + gap * 6, bw, bh, GREEN_BTN, sf::Color::White, mousePos, mouseClicked)) {
                char reportBuf[3000];
                hospital.generateDailyReport(reportBuf, 3000);
                showMessage("Daily Report", reportBuf, SCREEN_ADMIN_MENU);
            }
            if (drawButton(window, font, "10. Logout", bx, by + gap * 7, bw, bh, RED_BTN, sf::Color::White, mousePos, mouseClicked)) {
                loggedInId = -1; loggedInName[0] = '\0';
                currentScreen = SCREEN_LOGIN;
            }
        }

        // ---- ADMIN ADD DOCTOR ----
        else if (currentScreen == SCREEN_ADMIN_ADD_DOCTOR) {
            drawText(window, font, "Add New Doctor", 30, 70, 24, TITLE_COLOR);
            float fx = 80, fy = 110, fw = 700, fh = 40;
            if (mouseClicked) {
                sf::FloatRect r1(fx, fy, fw, fh);
                sf::FloatRect r2(fx, fy + 70, fw, fh);
                sf::FloatRect r3(fx, fy + 140, fw, fh);
                sf::FloatRect r4(fx, fy + 210, fw, fh);
                sf::FloatRect r5(fx, fy + 280, fw, fh);
                field1.active = r1.contains((float)mousePos.x, (float)mousePos.y);
                field2.active = r2.contains((float)mousePos.x, (float)mousePos.y);
                field3.active = r3.contains((float)mousePos.x, (float)mousePos.y);
                field4.active = r4.contains((float)mousePos.x, (float)mousePos.y);
                field5.active = r5.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, field1, fx, fy, fw, fh, field1Label);
            drawInputField(window, font, field2, fx, fy + 70, fw, fh, field2Label);
            drawInputField(window, font, field3, fx, fy + 140, fw, fh, field3Label);
            field4.isPassword = true;
            drawInputField(window, font, field4, fx, fy + 210, fw, fh, field4Label);
            drawInputField(window, font, field5, fx, fy + 280, fw, fh, field5Label);

            if (drawButton(window, font, "Add Doctor", fx, fy + 360, 200, 44, GREEN_BTN, sf::Color::White, mousePos, mouseClicked)) {
                char msg[300];
                float fee = myStrToFloat(field5.buf);
                hospital.addDoctor(field1.buf, field2.buf, field3.buf, field4.buf, fee, msg);
                showMessage("Result", msg, SCREEN_ADMIN_MENU);
            }
            if (drawButton(window, font, "Back", fx + 220, fy + 360, 150, 44, GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                currentScreen = SCREEN_ADMIN_MENU;
        }

        // ---- ADMIN VIEW ALL (used for remove doctor too) ----
        else if (currentScreen == SCREEN_ADMIN_VIEW_ALL) {
            drawText(window, font, "Remove Doctor", 30, 70, 24, TITLE_COLOR);
            float ly = 110;
            drawText(window, font, "All Doctors:", 30, ly, 15, DIM_TEXT);
            ly += 28;
            Storage<Doctor>& docs = hospital.getDoctors();
            for (int i = 0; i < docs.size(); i++) {
                float row = ly + i * 26 - scrollOffset;
                if (row > 380 || row < 100) continue;
                sf::Color rowBg = (i % 2 == 0) ? sf::Color(32, 32, 50) : sf::Color(28, 28, 45);
                drawRoundedRect(window, 20, row, 860, 24, rowBg);
                Doctor* d = docs.getAt(i);
                char idBuf[10]; myIntToStr(d->getId(), idBuf);
                char feeBuf[20]; myFloatToStr(d->getFee(), feeBuf);
                char line[300];
                myStrCopy(line, "ID:"); myStrCat(line, idBuf);
                myStrCat(line, " | "); myStrCat(line, d->getName());
                myStrCat(line, " | "); myStrCat(line, d->getSpecialization());
                myStrCat(line, " | PKR "); myStrCat(line, feeBuf);
                drawText(window, font, line, 28, row + 4, 13, TEXT_COLOR);
            }

            float fx = 80, fy = 420;
            if (mouseClicked) {
                sf::FloatRect r(fx, fy, 400, 40);
                field1.active = r.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, field1, fx, fy, 400, 40, field1Label);

            if (drawButton(window, font, "Remove Doctor", fx, fy + 70, 200, 44, RED_BTN, sf::Color::White, mousePos, mouseClicked)) {
                char msg[300];
                hospital.removeDoctor(myStrToInt(field1.buf), msg);
                showMessage("Result", msg, SCREEN_ADMIN_MENU);
            }
            if (drawButton(window, font, "Back", fx + 220, fy + 70, 150, 44, GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                currentScreen = SCREEN_ADMIN_MENU;
        }

        // ---- ADMIN DISCHARGE ----
        else if (currentScreen == SCREEN_ADMIN_DISCHARGE) {
            drawText(window, font, "Discharge Patient", 30, 70, 24, TITLE_COLOR);
            // Show all patients
            float ly = 110;
            Storage<Patient>& pats = hospital.getPatients();
            for (int i = 0; i < pats.size(); i++) {
                float row = ly + i * 24 - scrollOffset;
                if (row > 380 || row < 100) continue;
                Patient* p = pats.getAt(i);
                char idBuf[10]; myIntToStr(p->getId(), idBuf);
                char line[200];
                myStrCopy(line, "ID:"); myStrCat(line, idBuf);
                myStrCat(line, " | "); myStrCat(line, p->getName());
                myStrCat(line, " | Age:"); char ageBuf[5]; myIntToStr(p->getAge(), ageBuf);
                myStrCat(line, ageBuf);
                drawText(window, font, line, 30, row, 14, TEXT_COLOR);
            }

            float fx = 80, fy = 420;
            if (mouseClicked) {
                sf::FloatRect r(fx, fy, 400, 40);
                field1.active = r.contains((float)mousePos.x, (float)mousePos.y);
            }
            drawInputField(window, font, field1, fx, fy, 400, 40, field1Label);

            if (drawButton(window, font, "Discharge", fx, fy + 70, 200, 44, RED_BTN, sf::Color::White, mousePos, mouseClicked)) {
                char msg[300];
                hospital.dischargePatient(myStrToInt(field1.buf), msg);
                showMessage("Result", msg, SCREEN_ADMIN_MENU);
            }
            if (drawButton(window, font, "Back", fx + 220, fy + 70, 150, 44, GRAY_BTN, sf::Color::White, mousePos, mouseClicked))
                currentScreen = SCREEN_ADMIN_MENU;
        }

        // ---- MESSAGE SCREEN ----
        else if (currentScreen == SCREEN_MESSAGE) {
            drawText(window, font, msgTitle, 30, 70, 26, TITLE_COLOR);

            // Parse msg by \n and draw line by line
            float ly = 115;
            char lineBuf[300];
            int li = 0;
            const char* src = msgBody;
            while (*src != '\0') {
                if (*src == '\n' || *(src+1) == '\0') {
                    if (*(src+1) == '\0' && *src != '\n') lineBuf[li++] = *src;
                    lineBuf[li] = '\0';
                    float row = ly - scrollOffset;
                    if (row >= 65 && row < 600)
                        drawText(window, font, lineBuf, 30, row, 14, TEXT_COLOR);
                    ly += 22;
                    li = 0;
                } else {
                    lineBuf[li++] = *src;
                    if (li > 298) { lineBuf[li] = '\0'; li = 0; }
                }
                src++;
            }

            if (drawButton(window, font, "OK", 380, 600, 140, 38, GREEN_BTN, sf::Color::White, mousePos, mouseClicked)) {
                currentScreen = msgReturnScreen;
                scrollOffset = 0;
            }
            drawText(window, font, "Scroll with mouse wheel", 600, 610, 12, DIM_TEXT);
        }

        window.display();
    }

    return 0;
}
