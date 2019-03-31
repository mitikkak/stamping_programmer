
namespace stamping
{

class Buttons
{
public:
    void init();
    bool touched() const;
    bool up() const;
    bool down() const;
    bool select() const;
    bool back() const;
    void log() const;
    void clear();
private:
    static void upButtonCB();
    static void downButtonCB();
    static void selectButtonCB();
    static void backButtonCB();
    static int upValue;
    static int downValue;
    static volatile int selectValue;
    static int backValue;
};
}
