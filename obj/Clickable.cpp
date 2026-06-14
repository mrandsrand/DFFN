#include "Clickable.h"

Clickable::Clickable() {
    onClickListener = nullptr;
}

void Clickable::onClick() {
    if (isListenerSet()) onClickListener->onClick(*this);
}

void Clickable::setOnClickListener(OnClickListener *listener) {
    onClickListener = listener;
}

bool Clickable::isListenerSet() {
    return onClickListener != nullptr;
}
