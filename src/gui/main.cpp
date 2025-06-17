#include <QApplication>
#include <QMainWindow>
#include <QLabel>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle("PCB Autorouter GUI Test");
    mainWindow.setMinimumSize(400, 300);

    QLabel *label = new QLabel("Hello, CUDA PCB Autorouter!", &mainWindow);
    label->setAlignment(Qt::AlignCenter);
    mainWindow.setCentralWidget(label);

    mainWindow.show();
    return app.exec();
}