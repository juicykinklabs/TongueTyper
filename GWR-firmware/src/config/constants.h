#pragma once

// Filesystem path constants

namespace FSPATH {
    
    // system files
    constexpr char Settings[] = "/sys/setting.json";
    constexpr char Stats[] = "/sys/stats.json";
    
    // system folders
    constexpr char MediaFolder[] = "/sys/media/"; // with trailing slash
    constexpr char LogFolder[] = "/sys/logs/"; // with trailing slash
    
    // user folders
    constexpr char UserImagesFolder[] = "/usermedia/images/"; // with trailing slash
    constexpr char UserSoundsFolder[] = "/usermedia/sounds/"; // with trailing slash
}

namespace WIRELESS {
    constexpr char hostname[] = "ttyper"; // letters, numbers, and dashes only
}