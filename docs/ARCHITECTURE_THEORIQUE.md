# Architecture hybride — schéma théorique

Ce document représente des emplacements de modules à étudier. Aucun mécanisme de contournement d'anti-cheat, de camouflage de mémoire, de spoofing ou d'accès DMA arbitraire n'est implémenté.

## Couches

- `kernel/device_interface/` : contrôle d'accès, gestion des requêtes et contrat IOCTL.
- `kernel/telemetry/` : événements bornés et compteurs.
- `kernel/security/` : validation des entrées et invariants de sécurité.
- `external/process_observer/` : observation autorisée de processus de test.
- `external/event_pipeline/` : émission, agrégation et gestion des erreurs.
- `external/diagnostics/` : journaux et métriques.
- `common/schema/` : types et versions du protocole.
- `common/validation/` : spécifications de validation partagées.
- `research/dma_iommu/` : documentation DMA, IOMMU et isolation des périphériques.
- `research/secure_boot_hvci/` : exigences de signature, Secure Boot et HVCI.
- `research/memory_integrity/` : modèle des protections mémoire Windows.
- `research/anti_cheat_threat_model/` : surfaces de confiance et modèles de menace anti-triche, sans techniques d'évasion.
- `tests/` : scénarios de test IPC, pilote et simulation hors ligne.

## Flux abstrait

`Processus de test -> External -> IOCTL versionné -> Kernel -> compteurs/état -> External`

Les fichiers existants à la racine des modules restent intacts. Tous les nouveaux dossiers sont de simples réservations (`.gitkeep`).
