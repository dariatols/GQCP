target_sources(gqcp
    PRIVATE
        EvaluatableScalarRSQOneElectronOperator.hpp
        GSQOneElectronOperator.hpp
        RSQOneElectronOperator.hpp
        MixedUSQTwoElectronOperatorComponent.hpp
        OperatorTraits.hpp
        PureUSQTwoElectronOperatorComponent.hpp
        RSQOneElectronOperator.hpp
        RSQTwoElectronOperator.hpp
        SimpleSQOneElectronOperator.hpp
        SimpleSQTwoElectronOperator.hpp
        SQHamiltonian.hpp
        SQOperatorStorage.hpp
        SQOperatorStorageBase.hpp
        USQOneElectronOperator.hpp
        USQOneElectronOperatorComponent.hpp
        USQTwoElectronOperator.hpp
)

add_subdirectory(ModelHamiltonian)
