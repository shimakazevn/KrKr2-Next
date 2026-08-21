//
// Created by LiDon on 2025/9/15.
//
#pragma once

#include "ResourceManager.h"
#include "Player.h"

namespace motion {

    enum class MaskMode { MaskModeAlpha };

    class EmotePlayer : public Player {
    public:
        explicit EmotePlayer(ResourceManager rm) : Player() {}

        void initPhysics() {}

        void setMaskMode(tjs_int mode) { this->_maskMode = static_cast<MaskMode>(mode); }

        [[nodiscard]] tjs_int getMaskMode() const { return static_cast<tjs_int>(this->_maskMode); }

    private:
        MaskMode _maskMode = MaskMode::MaskModeAlpha;
    };

} // namespace motion