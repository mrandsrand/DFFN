// Clickable objects may have onClick event
class Clickable {

public:
    class OnClickListener {
    public:
        virtual ~OnClickListener() = default;
        virtual void onClick(Clickable &clickable) = 0;
    };

    void onClick();

    void setOnClickListener(OnClickListener *listener);

    bool isListenerSet();

protected:
    Clickable();

    OnClickListener *onClickListener;
};