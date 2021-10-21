<?php declare(strict_types=1);
/**
 * PHP Version 7
 *
 * @category ApiViews
 * @package  Loris
 * @license  http://www.gnu.org/licenses/gpl-3.0.txt GPLv3
 * @link     https://www.github.com/aces/Loris/
 */

namespace LORIS\api\Views;

/**
 * This class formats a Project object into arrays following the API
 * specifications.
 *
 * @category ApiViews
 * @package  Loris
 * @license  http://www.gnu.org/licenses/gpl-3.0.txt GPLv3
 * @link     https://www.github.com/aces/Loris/
 */

class Project_0_0_4_Dev
{
    /**
     * The project to format
     *
     * @var \Project
     */
    protected $_project;

    /**
     * Constructor
     *
     * @param \Project $project The project to format
     */
    public function __construct(\Project $project)
    {
        $this->_project = $project;
    }

    /**
     * Produce an array representation of this project.
     *
     * @return array
     */
    public function toArray(): array
    {
        $name = $this->_project->getName();

        return [
            'Id'                 => $this->_project->getId(),
            'Name'               => $this->_project->getName(),
            'Alias'              => $this->_project->getAlias(),
            'RecruitementTarget' => $this->_project->getRecruitmentTarget(),
            'Links'              => [
                'Candidates'  => "/projects/$name/candidates",
                'Instruments' => "/projects/$name/instruments",
                'Subprojects' => "/projects/$name/subprojects"
            ]
        ];
    }
}
