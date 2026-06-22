#include "fvCFD.H"
#include <cmath>

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

    Info << "Reading U field" << endl;

    volVectorField U
    (
        IOobject
        (
            "U",
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh
    );

    const vectorField& C = mesh.C();

    // -------------------------
    // Vortex-ring parameters
    // -------------------------

    const scalar R      = 1.0;
    const scalar sigma0 = 0.18;

    const scalar nu     = 1.0e-3;
    const scalar Gamma  = -500.0*nu;

    const scalar xc = 5.0;
    const scalar yc = 5.0;

    const scalar z1 = 2.0;
    const scalar z2 = 6.0;

    // -------------------------
    // Initialize velocity field
    // -------------------------

    forAll(U, cellI)
    {
        scalar x = C[cellI].x() - xc;
        scalar y = C[cellI].y() - yc;
        scalar z = C[cellI].z();

        scalar Ux = 0.0;
        scalar Uy = 0.0;
        scalar Uz = 0.0;

        // =====================================
        // Ring 1
        // =====================================

        {
            scalar zz = z - z1;

            scalar phi = std::atan2(y, x);

            scalar r = std::sqrt(x*x + y*y);

            scalar sigma =
                std::sqrt
                (
                    zz*zz
                  + (r - R)*(r - R)
                );

            if (sigma > SMALL)
            {
                scalar sigmaRatio = sigma/sigma0;

                scalar uSigma =
                    (Gamma
                    /(2.0*constant::mathematical::pi*sigma*sigma))
                   *(1.0 - std::exp(-sigmaRatio*sigmaRatio));

                scalar zeta =
                    std::atan2
                    (
                        zz,
                        (r - R)
                    );

                Ux += -uSigma*sigma
                    * std::sin(zeta)
                    * std::cos(phi);

                Uy += -uSigma*sigma
                    * std::sin(zeta)
                    * std::sin(phi);

                Uz +=  uSigma*sigma
                    * std::cos(zeta);
            }
        }

        // =====================================
        // Ring 2
        // =====================================

        {
            scalar zz = z - z2;

            scalar phi = std::atan2(y, x);

            scalar r = std::sqrt(x*x + y*y);

            scalar sigma =
                std::sqrt
                (
                    zz*zz
                  + (r - R)*(r - R)
                );

            if (sigma > SMALL)
            {
                scalar sigmaRatio = sigma/sigma0;

                scalar uSigma =
                   -(Gamma
                    /(2.0*constant::mathematical::pi*sigma*sigma))
                   *(1.0 - std::exp(-sigmaRatio*sigmaRatio));

                scalar zeta =
                    std::atan2
                    (
                        zz,
                        (r - R)
                    );

                Ux += -uSigma*sigma
                    * std::sin(zeta)
                    * std::cos(phi);

                Uy += -uSigma*sigma
                    * std::sin(zeta)
                    * std::sin(phi);

                Uz +=  uSigma*sigma
                    * std::cos(zeta);
            }
        }

        U[cellI] = vector(Ux, Uy, Uz);
    }

    U.correctBoundaryConditions();

    Info << "Writing U" << endl;

    U.write();
	
	
        volVectorField vort
    (
        IOobject
        (
            "vorticity",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        fvc::curl(U)
    );

    volScalarField magVorticity
    (
        IOobject
        (
            "magVorticity",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mag(vort)
    );
    vort.write();
    magVorticity.write();

    Info << "\nVortex-ring initialization complete\n" << endl;

    return 0;
}
