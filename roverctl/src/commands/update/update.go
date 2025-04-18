package command_update

import (
	"context"
	"fmt"

	"github.com/spf13/cobra"

	command_prechecks "github.com/VU-ASE/rover/roverctl/src/commands/prechecks"
	"github.com/VU-ASE/rover/roverctl/src/configuration"
	"github.com/VU-ASE/rover/roverctl/src/openapi"
	"github.com/VU-ASE/rover/roverctl/src/style"
	utils "github.com/VU-ASE/rover/roverctl/src/utils"
)

func Add(rootCmd *cobra.Command) {
	// General flags
	var roverIndex int
	var roverdHost string
	var roverdUsername string
	var roverdPassword string

	// Self-update command
	var version string
	var selfUpdateCmd = &cobra.Command{
		Use:   "update",
		Short: "Self-update roverctl and roverd",
		Long:  `Update roverctl and roverd to the latest version, or the version specified.`,
		RunE: func(cmd *cobra.Command, args []string) error {
			// Update roverd
			conn, er := command_prechecks.Perform(cmd, args, roverIndex, roverdHost, roverdUsername, roverdPassword)
			if er != nil {
				return er
			}

			// If the user specifies a version, update to that version
			// otherwise check the latest version of both roverctl and roverd
			if version == "" {
				update, err := utils.CheckForGithubUpdate("rover", "vu-ase", "")
				if err != nil {
					fmt.Printf("Could not check for updates: %s\nIf you want to update to a specific version, use the --version flag.\n", err)
					return nil
				}

				version = update.LatestVersion
			}
			version = utils.Version(version)
			fmt.Printf("Updating roverd and roverctl to version %s\n", style.Success.Render(version))

			api := conn.ToApiClient()
			update := api.HealthAPI.UpdatePost(
				context.Background(),
			)
			update = update.UpdatePostRequest(openapi.UpdatePostRequest{
				Version: version,
			})

			fmt.Printf("Updating roverd...\n")
			http, err := update.Execute()
			if err != nil {
				fmt.Printf("Could not update roverd: %s\n", utils.ParseHTTPError(err, http))
				return nil
			}
			fmt.Println("The roverd update was scheduled. This might take a while to complete. Do not interrupt the process.")

			// Update roverctl
			fmt.Printf("Updating roverctl...\n")
			utils.ExecuteShellCommand(configuration.ROVERCTL_UPDATE_LATEST_SCRIPT_WITH_VERSION + version)
			return nil
		},
	}
	selfUpdateCmd.Flags().IntVarP(&roverIndex, "rover", "r", 0, "The index of the rover to upload to, between 1 and 20")
	selfUpdateCmd.Flags().StringVarP(&roverdHost, "host", "", "", "The roverd endpoint to connect to (if not using --rover)")
	selfUpdateCmd.Flags().StringVarP(&roverdUsername, "username", "u", "debix", "The username to use to connect to the roverd endpoint")
	selfUpdateCmd.Flags().StringVarP(&roverdPassword, "password", "p", "debix", "The password to use to connect to the roverd endpoint")
	selfUpdateCmd.Flags().StringVarP(&version, "version", "v", "", "The version tag to update/downgrade to (e.g. v0.1.0)")
	addUpdateRoverctl(selfUpdateCmd)
	addUpdateRoverd(selfUpdateCmd)
	rootCmd.AddCommand(selfUpdateCmd)
}
